#include <iostream>

#include "metacontroller.hh"
#include "timestamp.hh"

using namespace std;


MetaController::MetaController( const bool debug,
    const float cwnd,
    const uint64_t rtt_thresh)
  : Controller ( debug ),
    cwnd_ (cwnd),
    rtt_thresh_ (rtt_thresh),
    min_rtt_ (500),
    rtt_window_ (RttWindow(debug)),
    delivery_window_ (DeliveryWindow(debug)),
    bw_window_ (BwWindow(debug))
{}


/* Get current window size, in datagrams */
unsigned int MetaController::window_size( void )
{
  unsigned int the_window_size = static_cast<unsigned int>(cwnd_);

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }
  return the_window_size;
}


/* An ack was received */
void MetaController::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Get latest RTT */
  uint64_t rtt_t = timestamp_ack_received - send_timestamp_acked;

  /* Update RTT samples */
  rtt_window_.update_rtt_samples(timestamp_ack_received, rtt_t);

  auto total_delivered = delivery_window_.get_curr_delivered() + 1;
  delivery_window_.update_delivery_data(timestamp_ack_received, total_delivered);
  auto delivered_at_sendts = delivery_window_.get_delivered(send_timestamp_acked);

  auto bw_t = (float)(total_delivered - delivered_at_sendts.second)/(timestamp_ack_received - delivered_at_sendts.first);

  bw_window_.update_bw_samples(timestamp_ack_received, bw_t);
  curr_max_bw_ = bw_window_.max_bw();
  min_rtt_ = rtt_window_.min_rtt();
  bdp_ = curr_max_bw_ * min_rtt_;
  if (debug_) {
    cerr << "time " << timestamp_ack_received << " bw_t " << bw_t << " pkt/s" << endl;
    cerr << "time " << timestamp_ack_received << " rtt_t " << rtt_t << " ms" << endl;
    cerr << "time " << timestamp_ack_received << " bw " << curr_max_bw_ << " pkts/ms" << endl;
    cerr << "time " << timestamp_ack_received << " rtt " << min_rtt_ << " ms" << endl;
    cerr << "time " << timestamp_ack_received << " bdp " << bdp_ << " pkts" << endl;
  }

  cwnd_ = 1.7 * bdp_;
  /* Find minimum RTT in the window */
  rtt_thresh_ = 1.5 * min_rtt_;

  /* Cwnd AIMD update */
  /*
  if (rtt_t < rtt_thresh_) {
    cwnd_ += 1/cwnd_;
  }
  else {
    if (cwnd_ > 4) {
      cwnd_ /= (rtt_t/min_rtt_);
    }
  }
  */
  if (rtt_t > min_rtt_) {
    cwnd_ /= (rtt_t/min_rtt_);
  }

  /* Ensure window >= 2 */
  cwnd_ = cwnd_ < 3 ? 3 : cwnd_;
  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << ", min_rtt " << min_rtt_
	 << ", min_rtt (w) " << rtt_window_.min_rtt()
	 << ", cwnd " << cwnd_ << endl;
  }
}

/* An ack was received */
void MetaController::timed_out()
  /* when time put happens */
{
    //cwnd_ = 3;

  if ( debug_ ) {
    cerr << "Timed out. cwnd: " << cwnd_ << endl;
  }
}

/* Wait for some time between sending packets */
float MetaController::get_interpkt_delay( void )
{
  return 1./curr_max_bw_;
}


/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int MetaController::timeout_ms( void )
{
  //return static_cast<unsigned int>(2.5 * min_rtt_);
  return 100;
}


/*************** RTT Window ******************/
RttWindow::RttWindow( const bool debug )
  : debug_ ( debug )
{}


/* Update RTT samples */
void RttWindow::update_rtt_samples( uint64_t curr_time,
        uint64_t rtt_sample ){
  rtt_samples_.push_back(std::make_pair(curr_time, rtt_sample));

  /* Remove old entries */
  if (curr_time < rtt_sample_window_) {
    return;
  }
  while (rtt_samples_.front().first < curr_time - rtt_sample_window_) {
    rtt_samples_.pop_front();
  }
}


/* Return minimum of the RTT samples */
uint64_t RttWindow::min_rtt( void ) {
  std::deque<pair<uint64_t, uint64_t>>::iterator min_elem = std::min_element(
      rtt_samples_.begin(),
      rtt_samples_.end(),
      [](std::pair<uint64_t, uint64_t> &left,
        std::pair<uint64_t, uint64_t> &right) { return left.second < right.second;});
  return min_elem->second;
}

/*************** Delivery Data ******************/
DeliveryWindow::DeliveryWindow( const bool debug )
  : debug_ ( debug )
{}


/* Update delivery data */
void DeliveryWindow::update_delivery_data( uint64_t curr_time,
        uint64_t delivered ){
  if (!delivery_data_.empty() &&
      delivery_data_.back().first == curr_time) {
    delivery_data_.back().second = delivered;
    if (debug_) {
      cerr << "updated " << curr_time << " " << delivered << endl;
    }
  }
  else {
    delivery_data_.push_back(std::make_pair(curr_time, delivered));
    if (debug_) {
      cerr << "pushed " << curr_time << " " << delivered << endl;
    }
  }

  /* Update total delivered so far */
  assert(delivered >= last_delivered_);
  last_delivered_ = delivered;

  /* Remove old entries */
  if (curr_time < delivery_data_window_) {
    return;
  }
  while (delivery_data_.front().first < curr_time - delivery_data_window_) {
    delivery_data_.pop_front();
  }
}

/* Get delivered so far */
const uint64_t& DeliveryWindow::get_curr_delivered( void ) {
  return last_delivered_;
}

/* Get delivered on of before a given time */
std::pair<uint64_t,uint64_t> DeliveryWindow::get_delivered( const uint64_t& timestamp ) {
  std::deque<pair<uint64_t, uint64_t>>::iterator upper = std::upper_bound(
      delivery_data_.begin(),
      delivery_data_.end(),
      std::make_pair(timestamp, timestamp),
      [](std::pair<uint64_t, uint64_t> left,
        std::pair<uint64_t, uint64_t> right) { return left.first < right.first;}
      );
  if (upper == delivery_data_.begin()){
    return std::make_pair(0UL, 0UL);
  }
  auto last_entry_on_before_ts = std::prev(upper);
  return *last_entry_on_before_ts;
}

/*************** BW Window ******************/
BwWindow::BwWindow( const bool debug )
  : debug_ ( debug )
{}


/* Update BW samples */
void BwWindow::update_bw_samples( uint64_t curr_time,
        float bw_sample ){
  bw_samples_.push_back(std::make_pair(curr_time, bw_sample));

  /* Remove old entries */
  if (curr_time < bw_sample_window_) {
    return;
  }
  while (bw_samples_.front().first < curr_time - bw_sample_window_) {
    bw_samples_.pop_front();
  }
}

/* Return max of the BW samples */
float BwWindow::max_bw( void ) {
  std::deque<pair<uint64_t, float>>::iterator max_elem = std::max_element(
      bw_samples_.begin(),
      bw_samples_.end(),
      [](std::pair<uint64_t, float> &left,
        std::pair<uint64_t, float> &right) { return left.second < right.second;});
  return max_elem->second;
}

