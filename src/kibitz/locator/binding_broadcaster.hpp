#ifndef kibitz_binding_broadcaster_hpp
#define kibitz_binding_broadcaster_hpp

#include <kibitz/validator/validator.hpp>
#include <kibitz/publisher.hpp>

namespace kg = kibitz::graph;

namespace kibitz {
  /// \brief Broadcasts collaboration graph bindings to workers.
  ///
  /// Periodically publishers bindings that each worker type 
  /// needs to connect and recieve collaboration messages.
  class binding_broadcaster {
    publisher publisher_;
    kibitz::graph::worker_graph_ptr worker_graph_;

  public:
    /// \brief c'tor
    /// \param pub object used to publish messages
    /// \param worker_graph_ptr object containing information about collaboration graph
    ///
    binding_broadcaster( const publisher& pub, kg::worker_graph_ptr worker_graph_ptr );
    ~binding_broadcaster();

    /// \brief thread function, controls timing of binding information broadcasts.
    void operator()();
  };

}
#endif
