#ifndef kibitz_locator_heartbeat_generator_hpp
#define kibitz_locator_heartbeat_generator_hpp

#include <kibitz/publisher.hpp>

namespace kibitz {
  namespace locator {
    /// Class publishes heartbeat messages periodically.
    class heartbeat_generator {
      publisher publisher_;
      const int frequency_ms_;
      const int publish_port_;
    public:
      /// c'tor
      /// \param pub messaging bus that will be used to send heartbeats.
      /// \param frequency_ms heartbeat send frequency in milliseconds.
      /// \param port heartbeats will be published on this port
      heartbeat_generator( const publisher& pub, int frequency_ms, int port  ); 
      ~heartbeat_generator() ;
      /// Thread function
      void operator()();
    };
  }
}

#endif
