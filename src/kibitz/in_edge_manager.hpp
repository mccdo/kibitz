/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * Copyright 2012-2012 by Ames Laboratory
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.rb END do not edit this line> ***************/
#ifndef in_edge_manager_hpp
#define in_edge_manager_hpp

#include <kibitz/common.hpp>
#include <kibitz/messages/worker_info.hpp>
#include <kibitz/messages/notification_message.hpp>
#include <kibitz/kibitz_util.hpp>

namespace kibitz
{
  /// \brief Handles messages coming from parent worker nodes
  ///
class in_edge_manager
{
  struct notification_context_t {
    string last_binding;
    int count_items;
    int size_items;
    zmq_pollitem_t* pollitems;
  };

    struct collaboration_context_t
    {
        int count_items;
        std::map< string, std::map< string, string >  > job_messages;
        zmq_pollitem_t* pollitems;
        std::vector<string> get_job_messages( const string& job_id )
        {
            std::vector<string> result;
            for( std::map<string, string>::iterator it = job_messages[job_id].begin(); it != job_messages[job_id].end(); ++it )
            {
                result.push_back( it->second );
            }
            return result;
        }
    };

    context& context_;
    const string worker_type_ ;
    const int worker_id_;
  util::sockman_ptr_t notification_socket_;
  void create_collaboration_binding( notification_context_t& context, notification_message_ptr_t message ) ;
    void create_bindings( zmq_pollitem_t** pollitems, int& count_items, int& size_items );
    void create_binding( const worker_info& info, zmq_pollitem_t& pollitem );
    void release_bindings( zmq_pollitem_t* pollitems, int count_items ) ;
  void handle_notification_message( notification_context_t& notification_context ); 
    void handle_collaboration_message( collaboration_context_t& context );
    bool all_messages_arrived( const string& job_id, collaboration_context_t& collab_context ) const;
    void check_and_start_job( notification_message_ptr_t message ) ;
public:
    in_edge_manager( context& ctx );
  /// Copy c'tor - we provide this because we don't want to 
  /// copy internal zmq sockets between threads
  in_edge_manager( const in_edge_manager& iem );
    virtual ~in_edge_manager() ;

  /// Thread functor
    void operator()() ;

  /// Used to communicate from other threads to
  /// the in edge manager
  ///
  /// \param json message to send to this thread
  ///
  void send_notification(  const string& json ) ;
  

  static const char* NOTIFICATION_BINDING;

};
}

#endif
