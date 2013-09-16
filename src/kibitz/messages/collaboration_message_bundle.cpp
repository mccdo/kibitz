#include <kibitz/messages/collaboration_message_bundle.hpp>
#include <boost/foreach.hpp>

namespace kibitz
{

const char* collaboration_message_bundle::MESSAGE_TYPE = "collaboration_messages";

////////////////////////////////////////////////////////////////////////////////
collaboration_message_bundle::collaboration_message_bundle(
    const collaboration_message_ptrs_t& messages )
    :
    collaboration_message( collaboration_message_bundle::MESSAGE_TYPE ),
    messages_( messages )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
  collaboration_message_bundle::collaboration_message_bundle(  JSON::Object::Ptr json )
    :collaboration_message( json ) {

  JSON::Array::Ptr messages;
  get_value( json, "messages", messages );
  for( JSON::Array::ConstIterator i = messages->begin(); i != messages->end() ; ++i ) {
    basic_collaboration_message_ptr_t ptr = basic_collaboration_message::create( i->extract<JSON::Object::Ptr>() );
    messages_.push_back( ptr );
  }

}
////////////////////////////////////////////////////////////////////////////////
collaboration_message_bundle::~collaboration_message_bundle()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
string collaboration_message_bundle::to_json() const
{
    stringstream stm;
    JSON::Object::Ptr json;
    read_json( "{}", json );
    collaboration_message::populate_header( json );
    JSON::Array messages;

    BOOST_FOREACH( basic_collaboration_message_ptr_t message, messages_ )
    {
      JSON::Object::Ptr obj;
      read_json( "{}", obj );
      message->to_json( obj );
      messages.add( obj );
    }

    json->set( "messages", messages );
    json->stringify( stm );
    return stm.str();
}
////////////////////////////////////////////////////////////////////////////////

} //end kibitz
