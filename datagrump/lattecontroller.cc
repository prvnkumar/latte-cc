#include <iostream>

#include "lattecontroller.hh"
#include "timestamp.hh"

using namespace std;

LatteController::LatteController( const bool debug )
  : Controller ( debug ),
    rtt_window_ ( RttWindow(debug) ),
    delivery_window_ ( DeliveryWindow(debug) ),
    bw_window_ ( BwWindow(debug) )
{}

LatteController::LatteController( const bool debug,
    const float lambda)
  : Controller ( debug ),
    lambda_ ( lambda ),
    rtt_window_ ( RttWindow(debug) ),
    delivery_window_ ( DeliveryWindow(debug) ),
    bw_window_ ( BwWindow(debug) )
{}


/* Get current window size, in datagrams */
unsigned int LatteController::window_size( void )
{
  unsigned int the_window_size = static_cast<unsigned int>(cwnd_);

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }
  return the_window_size;
}

/* An ack was received */
void LatteController::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  // auto prev_cwnd = cwnd_;

  /* Get latest RTT */
  uint64_t rtt_t = timestamp_ack_received - send_timestamp_acked;

  auto rtt_grad_t = ((float)rtt_t - rtt_window_.last_rtt())/min_rtt_;
  rtt_grad_ = (1 - 0.7) * rtt_grad_ + 0.7 * rtt_grad_t;

  /* Update RTT samples */
  rtt_window_.update_rtt_samples(timestamp_ack_received, rtt_t);

  auto total_delivered = delivery_window_.get_curr_delivered() + 1;
  delivery_window_.update_delivery_data(timestamp_ack_received, total_delivered);
  auto delivered_at_sendts = delivery_window_.get_delivered(send_timestamp_acked);

  auto bw_t = (float)(total_delivered - delivered_at_sendts.second)/(timestamp_ack_received - delivered_at_sendts.first);
  sbw_t_ = 0.3 * bw_t + 0.7 * sbw_t_;
  bw_window_.update_bw_samples(timestamp_ack_received, bw_t);
  curr_max_bw_ = bw_window_.max_bw();
  min_rtt_ = rtt_window_.min_rtt();
  bdp_ = curr_max_bw_ * min_rtt_;

  if (debug_) {
    cerr << "time " << timestamp_ack_received
      << " bw_t " << bw_t << " pkt/s"
      << " rtt_t " << rtt_t << " ms"
      << ", min_rtt " << min_rtt_
      << " max_bw " << curr_max_bw_ << " pkts/ms"
      << " rtt " << min_rtt_ << " ms"
      << " bdp " << bdp_ << " pkts"
      << " rtt_grad " << rtt_grad_
	    << " , cwnd " << cwnd_ << endl;
  }

  if (cwnd_ > 10) {
    lambda_ = 1.6 + 0.4 * 10.0/cwnd_;
  }
  else {
    lambda_ = 2.0;
  }
  cwnd_ =  lambda_ * bdp_;

  if (rtt_t > min_rtt_) { /* Mostly true */
    cwnd_ /= ((float)rtt_t/min_rtt_);
  }

  if (rtt_grad_ > 0.1) {
    cwnd_ = cwnd_ * (1 - rtt_grad_);
    //cerr << rtt_grad_ << "\t" << cwnd_ << endl;
  }
  /* Ensure window >= 3 */
  cwnd_ = cwnd_ < 3 ? 3 : cwnd_;


  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << ", min_rtt (w) " << rtt_window_.min_rtt()
	 << ", cwnd " << cwnd_ << endl;
  }
}

/* An ack was received */
void LatteController::timed_out()
  /* when time out happens */
{
    //cwnd_ = 3;

  if ( debug_ ) {
    cerr << "Timed out. cwnd: " << cwnd_ << endl;
  }
}

/* Wait for some time between sending packets */
float LatteController::get_interpkt_delay( void )
{
  //return 1./curr_max_bw_ * gamma_ * 1000;
  if (sbw_t_ < 0.8 * curr_max_bw_) {
    return 1./curr_max_bw_ * gamma_ * 1000;
  }
  return 1./sbw_t_ * gamma_ * 1000;
}


/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int LatteController::timeout_ms( void )
{

 return static_cast<unsigned int>(1.5 * min_rtt_);
}
