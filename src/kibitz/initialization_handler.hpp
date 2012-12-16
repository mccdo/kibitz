#ifndef kibitz_initialization_handler_hpp
#define kibitz_initialization_handler_hpp

#include <kibitz/context.hpp>

namespace kibitz {

  /// Handles job initialization message on a seperate thread.
  class initialization_handler {
    context* context_;
    string initialization_data_;
  public:
    /// c'tor
    /// 
    /// \param ctx pointer to application context
    /// \param initialization_data payload from job initialization message
    ///
    initialization_handler( context* ctx, const string& initialization_data ) ;
    ~initialization_handler();

    void operator() ();
  };
}

#endif
