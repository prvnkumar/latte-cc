#ifndef AIMDCONTROLLER_HH
#define AIMDCONTROLLER_HH

#include <cstdint>

#include "controller.hh"
/* Congestion controller interface */

class AimdController : public Controller
{
private:
  float cwnd_; /* Congestion window */
  float aimd_inc_param_; /* Increase cwnd by this per RTT */
  float aimd_dec_param_; /* Decrease cwnd by this factor per timeout */

  /* Add member variables here */

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  AimdController( const bool debug);

  AimdController( const bool debug,
      const float cwnd,
      const int aimd_inc_param,
      const int aimd_dec_param);

  /* Timeout occured*/
  void timed_out( void );
  /* Get current window size, in datagrams */
  unsigned int window_size( void );

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms( void );

};

#endif
