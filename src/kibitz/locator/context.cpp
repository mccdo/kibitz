#include <zmq.h>
#include <kibitz/locator/context.hpp>


namespace locator
{
context::context()
    : zmq_context_( NULL ),
      zmq_context_thrds_( context::DEFAULT_CONTEXT_THREADS )
{
}

context::~context()
{
    if( zmq_context_ )
    {
        zmq_term( zmq_context_ ) ;
    }
}
}
