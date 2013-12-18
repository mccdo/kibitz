#ifndef kibitz_binding_broadcaster_hpp
#define kibitz_binding_broadcaster_hpp


#include <kibitz/publisher.hpp>
#include <kibitz/locator/common.hpp>

#include <kibitz/export_config.hpp>

#include <kibitz/logging.hpp>

namespace kg = kibitz::graph;
using kibitz::locator::binding_map_t;

namespace kibitz
{
namespace locator
{
/// \brief Broadcasts collaboration graph bindings to workers.
///
/// Periodically publishers bindings that each worker type
/// needs to connect and recieve collaboration messages.
class KIBITZ_LOCATOR_EXPORT binding_broadcaster
{
    publisher publisher_;
    binding_map_t bindings_;
    int send_frequency_;
public:
    /// \brief c'tor
    /// \param pub object used to publish messages
    /// \param bindings map of worker type names to ZMQ bindings
    /// \param send_frequency (default 1 sec) Given in milliseconds,
    /// defines how often binding information will be send to workers
    binding_broadcaster( const publisher& pub, binding_map_t& bindings, int send_frequency = 1000 );
    ~binding_broadcaster();

    /// \brief thread function, controls timing of binding information broadcasts.
    void operator()();
private:
    ///Logger utilities for debugging
    Poco::Logger& m_logger;
    ///Logger utilities for debugging
    LogStreamPtr m_logStream;
};
}

}
#endif
