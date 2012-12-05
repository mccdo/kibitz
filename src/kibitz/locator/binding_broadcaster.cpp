#include <kibitz/locator/binding_broadcaster.hpp>

namespace kibitz {
  namespace locator {

    binding_broadcaster::binding_broadcaster( const publisher& pub, binding_map_t& bindings ) 
      :publisher_( pub ),
       bindings_( bindings ) {
    }

    binding_broadcaster::~binding_broadcaster() {
    }

    void binding_broadcaster::operator()() {
    }
  }
}
