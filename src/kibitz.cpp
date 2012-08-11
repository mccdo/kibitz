#include "kibitz.hpp"
#include "heartbeat_sender.hpp"
#include "context.hpp"


namespace kibitz {
  static context_ptr context_;

  void validate_command_line( const po::variables_map& command_line ) {

    if( !command_line.count( "worker-id" ) ) {
      throw runtime_error( string("Missing required worker-id argument") );
    }

    if( !command_line.count( "worker-type" ) ) {
      throw runtime_error(  string("Missing required worker-type argument") );
    }

  } 

  void initialize( int argc, char* argv[] ) {
    assert( !context_ );
    InitGoogleLogging( argv[0] );
    InstallFailureSignalHandler();
    DLOG(INFO) << "initialize start";
    fs::path path( argv[0] );
    stringstream ss ;
    ss << path.filename() << " options" ;
    po::options_description options(ss.str());
    options.add_options() 
      ( "help,h", "Show help message" )
      ( "worker-id,I", po::value< int >(), "(Required) Integer that identifies worker. Must be unique with worker type" )
      ( "worker-type,T", po::value< string >(), "(Required) Name of the type of worker." )
      ( "context-threads,t", po::value< int >()->default_value( 1 ), "Thread count passed to zmq_init" )
      ;
    po::variables_map command_line;
    po::store( po::parse_command_line( argc, argv, options ), command_line );
    po::notify( command_line );


    if( command_line.count( "help" ) ) {
      std::cout << options << std::endl;
      exit( 1 );
    }
    

    validate_command_line( command_line );
 
    context_=  context_ptr( new context( command_line ) );
  }

  void start() {
    assert( context_ );

  }

  void terminate() {
    if( context_ ) {
      context_->terminate();
    }
  }
  

}

