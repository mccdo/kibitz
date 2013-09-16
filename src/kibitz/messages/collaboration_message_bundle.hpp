#ifndef messages_collaboration_message_bundle_hpp
#define messages_collaboration_message_bundle_hpp

#include <kibitz/messages/basic_collaboration_message.hpp>

namespace kibitz {
  typedef std::vector< basic_collaboration_message_ptr_t > collaboration_message_ptrs_t;
  /// \brief Contains group of collaboration messages.
  ///
  /// The purpose of this class is to group collaborations by job id,
  /// and the worker id of the target worker or terminus of in edges. 
  ///
  class KIBITZ_MESSAGE_EXPORT collaboration_message_bundle : public collaboration_message {
    collaboration_message_ptrs_t messages_;
  public:
    collaboration_message_bundle( const collaboration_message_ptrs_t& messages ) ;
    collaboration_message_bundle( JSON::Object::Ptr json );
    virtual ~collaboration_message_bundle( );

    const collaboration_message_ptrs_t& messages() const { return messages_; }
    string to_json() const ;

    static const char* MESSAGE_TYPE;
  };

  typedef shared_ptr<collaboration_message_bundle> collaboration_message_bundle_ptr_t;
}

#endif
