#ifndef registry_hpp
#define registry_hpp

#include <zmq.h>
#include <map>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "heartbeat.hpp"
#include <sys/time.h>

using std::string;


class registry {

  typedef std::set< kibitz::heartbeat > worker_set_t;
  typedef std::map< string, worker_set_t > worker_map_t;


  void* publisher_socket_;
  void* inproc_pub_socket_;
  void* inproc_sub_socket_; 


  bool one_second_elapsed( timeval& last_send ); 
  
public:
  registry(  void* inproc_pub_socket, void* inproc_sub_socket, void* publisher_socket ); 
  virtual ~registry() ;
  void push_message( const string& msg ) ;
  void push_message( const kibitz::message& message );
  void operator() () ;


};

#endif
