#ifndef LATTECONTROLLER_HH
#define LATTECONTROLLER_HH

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>

#include "controller.hh"
#include "metacontroller.hh"


/* Congestion controller interface */
class LatteController : public Controller
{
protected:
  float cwnd_{50}; /* Congestion window */
  float min_rtt_{500};   /* Min RTT seen */
  float bdp_{10};
  float curr_max_bw_{10};
  float rtt_grad_{0};
  float sbw_t_{10};

  float lambda_{2.0};
  float gamma_{0.8};

  RttWindow rtt_window_;
  DeliveryWindow delivery_window_;
  BwWindow bw_window_;
  bool conservative_mode_{false};

public:

  LatteController( const bool debug );


  LatteController( const bool debug,
      const float cwnd );

  /* Timeout occured*/
  void timed_out( void );

  /* Wait between packets */
  float get_interpkt_delay( void );

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
