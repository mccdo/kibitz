#ifndef kibitz_router_hpp
#define kibitz_router_hpp

#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <kibitz/publisher.hpp>
#include <kibitz/logging.hpp>

#include <kibitz/validator/worker_graph.hpp>

#include <kibitz/locator/common.hpp>

#include <kibitz/export_config.hpp>

#include <string>
#include <map>
#include <utility>

using kibitz::graph::worker_graph_ptr;
using kibitz::basic_collaboration_message_ptr_t;

using kibitz::graph::node_ptr_t;
using kibitz::graph::node_names_t;

using kibitz::locator::binding_map_t;

namespace kibitz
{
namespace locator
{

typedef std::string inedge_worker_name_t;
typedef std::string target_worker_name_t;
typedef std::string jobid_name_t;

typedef std::map<inedge_worker_name_t, basic_collaboration_message_ptr_t> inedge_messages_t;
typedef std::pair<inedge_worker_name_t, basic_collaboration_message_ptr_t> inedge_message_pair_t;
typedef std::map<jobid_name_t, inedge_messages_t> jobids_messages_t;
typedef std::map< target_worker_name_t, jobids_messages_t > messages_by_worker_and_job_t;


/// \brief Listens for collaboration messages and distributes them to child workers.
///
class KIBITZ_LOCATOR_EXPORT router
{
    void* context_;
    std::string listener_binding_;
    binding_map_t push_bindings_;
    worker_graph_ptr graph_ptr_;
    publisher publisher_;
    typedef std::map< std::string, kibitz::util::sockman_ptr_t > send_sockets_t;

    void bind_out_sockets( send_sockets_t& send_sockets );

    void route_message( const send_sockets_t& send_sockets, const std::string& json, messages_by_worker_and_job_t& inedge_cache ) ;
    bool inedges_have_messages( collaboration_message_bundle_ptr_t messages ) const;
public:

    /// c'tor
    /// \param context ZMQ context
    /// \param pub publisher used to send messages out to the heart beat transmitter in a thread safe manner
    /// \param listener_binding ZMQ binding std::string for socket that will listen for inbound
    ///                         messages from workers.
    /// \param bindings bindings for outbound messages, these are generated by the create_bindings function
    /// \param graph_ptr contains collaboration graph information used to route messages
    ///
    router( void* context,
            const publisher& pub,
            const std::string& listener_binding,
            const binding_map_t& bindings,
            worker_graph_ptr graph_ptr );
    ~router() ;
    /// Thread worker function
    void operator()() ;

    /// \brief Used to check if all inedges are populated for a worker.
    /// If all inedges have a collaboration message associated with them,
    /// a composite message is returned with can be sent to worker at
    /// terminus of inedges.
    ///
    /// \param target_worker the worker that the message is being sent to
    /// \param graph information about collaboration graph
    /// \param new_message collaboration message
    /// \param cache [in,out] contains previously arrived collaboration messages
    /// for workers and job ids. If in edges are fully populated, the pertainant
    /// job will be removed.
    /// \return If all in edges for a particular job and worker have been populated
    /// a message containing all those messages will be returned.
    collaboration_message_bundle_ptr_t populate_inedge_messages(
        const target_worker_name_t& target_worker,
        worker_graph_ptr graph,
        basic_collaboration_message_ptr_t new_message,
        messages_by_worker_and_job_t& cache ) const;
private:
    ///Logger utilities for debugging
    Poco::Logger& m_logger;
    ///Logger utilities for debugging
    LogStreamPtr m_logStream;
};
}
}


#endif
