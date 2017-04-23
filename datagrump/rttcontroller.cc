#include <iostream>

#include "rttcontroller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
RttController::RttController( const bool debug )
  : Controller ( debug ),
    cwnd_ (50),
    rtt_thresh_ (500)
{}

RttController::RttController( const bool debug,
    const float cwnd,
    const uint64_t rtt_thresh)
  : Controller ( debug ),
    cwnd_ (cwnd),
    rtt_thresh_ (rtt_thresh)
{}


/* Get current window size, in datagrams */
unsigned int RttController::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
  unsigned int the_window_size = static_cast<unsigned int>(cwnd_);

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }

  return the_window_size;
}

/* An ack was received */
void RttController::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Additive increase */
  uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  if (rtt < rtt_thresh_) {
    cwnd_ += 1/cwnd_;
  }
  else {
    if (cwnd_ > 1) {
      cwnd_ -= 1/cwnd_;
    }
  }

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << ", cwnd " << cwnd_ << endl;
  }
}

/* An ack was received */
void RttController::timed_out()
  /* when time put happens */
{
  /* Multiplicative decrease */
  if (cwnd_ > 1) {
    cwnd_ /= 2;
  }

  if ( debug_ ) {
    cerr << "Timed out. cwnd: " << cwnd_ << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int RttController::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
