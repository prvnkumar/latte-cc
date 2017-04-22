#include <iostream>

#include "aimdcontroller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
AimdController::AimdController( const bool debug )
  : Controller ( debug ),
    cwnd_ (50),
    aimd_inc_param_ (0),
    aimd_dec_param_ (1)
{}

AimdController::AimdController( const bool debug,
    const float cwnd,
    const int aimd_inc_param,
    const int aimd_dec_param)
  : Controller ( debug ),
    cwnd_ (cwnd),
    aimd_inc_param_ (aimd_inc_param),
    aimd_dec_param_ (aimd_dec_param)
{}


/* Get current window size, in datagrams */
unsigned int AimdController::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
  unsigned int the_window_size = static_cast<unsigned int>(cwnd_);

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }

  return the_window_size;
}

/* A datagram was sent */
void AimdController::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void AimdController::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Additive increase */
  cwnd_ += aimd_inc_param_/cwnd_;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << ", cwnd " << cwnd_ << endl;
  }
}

/* An ack was received */
void AimdController::timed_out()
  /* when time put happens */
{
  /* Multiplicative decrease */
  if (cwnd_ > 1) {
    cwnd_ /= aimd_dec_param_;
  }

  if ( debug_ ) {
    cerr << "Timed out. cwnd: " << cwnd_ << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int AimdController::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
