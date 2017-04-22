#ifndef RTTCONTROLLER_HH
#define RTTCONTROLLER_HH

#include <cstdint>

#include "controller.hh"
/* Congestion controller interface */

class RttController : public Controller
{
private:
  float cwnd_; /* Congestion window */
  float rtt_thresh_; /* RTT threshold */


public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  RttController( const bool debug);

  RttController( const bool debug,
      const float cwnd,
      const uint64_t rtt_thresh);

  /* Timeout occured*/
  void timed_out( void );

  /* Get current window size, in datagrams */
  unsigned int window_size( void );

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
