#include <iostream>

#include "rttaimdcontroller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
RttAimdController::RttAimdController( const bool debug )
  : RttController ( debug )
{}

RttAimdController::RttAimdController( const bool debug,
    const float cwnd,
    const uint64_t rtt_thresh)
  : RttController ( debug, cwnd, rtt_thresh)
{}


/* An ack was received */
void RttAimdController::ack_received( const uint64_t sequence_number_acked,
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
    if (cwnd_ >= 2) {
      cwnd_ = cwnd_/2;
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
