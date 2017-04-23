#ifndef RTTAIMDCONTROLLER_HH
#define RTTAIMDCONTROLLER_HH

#include <cstdint>

#include "rttcontroller.hh"
/* Congestion controller interface */

class RttAimdController : public RttController
{

public:

  RttAimdController( const bool debug);

  RttAimdController( const bool debug,
      const float cwnd,
      const uint64_t rtt_thresh);

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

};

#endif
