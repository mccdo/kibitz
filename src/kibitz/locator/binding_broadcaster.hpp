#ifndef kibitz_binding_broadcaster_hpp
#define kibitz_binding_broadcaster_hpp


#include <kibitz/publisher.hpp>
#include <kibitz/locator/common.hpp>

namespace kg = kibitz::graph;
using kibitz::locator::binding_map_t;

namespace kibitz {
  namespace locator {
    /// \brief Broadcasts collaboration graph bindings to workers.
    ///
    /// Periodically publishers bindings that each worker type 
    /// needs to connect and recieve collaboration messages.
    class binding_broadcaster {
      publisher publisher_;
      binding_map_t bindings_;

    public:
      /// \brief c'tor
      /// \param pub object used to publish messages
      /// \param bindings map of worker type names to ZMQ bindings
      ///
      binding_broadcaster( const publisher& pub, binding_map_t& bindings );
      ~binding_broadcaster();

      /// \brief thread function, controls timing of binding information broadcasts.
      void operator()();
    };
  }

}
#endif
