#ifndef METACONTROLLER_HH
#define METACONTROLLER_HH

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>

#include "controller.hh"

class RttWindow
{
  private:
    bool debug_;
    uint64_t rtt_sample_window_ {5000};
    std::deque<std::pair<uint64_t, uint64_t>> rtt_samples_ {};

  public:

    RttWindow( const bool debug);
    /* Update RTT samples */
    void update_rtt_samples( uint64_t curr_time, uint64_t rtt_sample );

    /* Return minimum of the RTT samples */
    uint64_t min_rtt( void );
};

class BwWindow
{
  private:
    bool debug_;
    uint64_t bw_sample_window_ {200};
    std::deque<std::pair<uint64_t, float>> bw_samples_ {};

  public:

    BwWindow( const bool debug);

    /* Update BW samples */
    void update_bw_samples( uint64_t curr_time, float bw_sample );

    /* Return max of the BW samples */
    float max_bw( void );

};

class DeliveryWindow
{
  private:
    bool debug_;
    uint64_t delivery_data_window_ {5000};
    std::deque<std::pair<uint64_t, uint64_t>> delivery_data_ {};
    uint64_t last_delivered_{0};

  public:

    DeliveryWindow( const bool debug);

    /* Update delivery data */
    void update_delivery_data( uint64_t curr_time, uint64_t delivered);

    /* Get delivered so far */
    const uint64_t& get_curr_delivered( void );

    /* Get delivered on of before a given time */
    std::pair<uint64_t,uint64_t> get_delivered( uint64_t const& timestamp );

};

/* Congestion controller interface */
class MetaController : public Controller
{
protected:
  float cwnd_; /* Congestion window */
  float rtt_thresh_; /* RTT threshold */
  float min_rtt_;   /* Min RTT seen */
  float bdp_{100};
  float curr_max_bw_{100};

  float lambda_{1.7};
  float gamma_{0.8};

  RttWindow rtt_window_;
  DeliveryWindow delivery_window_;
  BwWindow bw_window_;
  bool conservative_mode_{false};

public:
  MetaController( const bool debug,
      const float cwnd,
      const uint64_t rtt_thresh);

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
