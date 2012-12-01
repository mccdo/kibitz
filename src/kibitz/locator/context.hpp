#ifndef locator_context_hpp
#define locator_context_hpp

#include <boost/property_tree.hpp>

using boost::ptree;

namespace locator {
  class context {
    
    void* zmq_context_;
    int zmq_context_thrds_ ;

    ptree configuration_;

    context();
  public:

    static const int DEFAULT_CONTEXT_THREADS = 2;


    ~context() ;
    
  };

  typedef boost::shared_ptr< context > context_ptr_t ;

 

}



#endif
