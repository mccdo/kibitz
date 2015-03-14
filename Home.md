

Kibitz is a grid computing platform that is designed to make it easy to build loosely coupled resilient grid computing systems.  Individual units of function called tasks are hosted in processes called [workers](Worker.md).  Workers participate in [collaborations](Collaboration.md) to perform jobs.  These collaborations can be constructed in arbitrary ways so long as all workers exchanging messages understand what to do with the message content that they receive from other workers.  Kibitz provides a host container that abstracts away the details of participating in the grid.  The Kibitz container provides [discovery](Discovery.md), logging, daemonization, communication and security services.  Tasks are units of logic that are hosted in a Kibitz container. To implement a task, a developer links his or her project to the Kibitz shared library and implements [CallbackFunctions](CallbackFunctions.md) that capture incoming messages sent from other workers.  On task completion send\_out\_message or send\_notification\_message methods are called to signal the completion of the task and optionally send messages to other workers.  Here is an example of a program that implements the kibitz library.

```
#include "kibitz.hpp"

void message_handler( const kibitz::collaboration_messages_t& messages ) ; 

int main( int argc, char* argv[] ) {
  std::cout << "starting" << std::endl;
  int result = 0;

  try {
    kibitz::initialize( argc, argv );
    // set callback that will be invoked when worker gets collaboration messages
    // from other workers
    kibitz::set_in_message_handler( message_handler );
    kibitz::start();
    kibitz::terminate();
  } catch( std::exception& e ) {
    result = 1;
    std::cout << "Program failed. " << e.what() << std::endl;
  }
  
  return result;
}

////////////////////////////////////////////////////////////////////////
// Callback function invoked when messages have arrived from parent workers.
// It receives an vector of messages, one for each parent worker.  
////////////////////////////////////////////////////////////////////////
void message_handler( const kibitz::collaboration_messages_t& messages )  {

  string payload;
  /* your custom worker logic goes here */
  // send collaboration message to child workers  
  kibitz::send_out_message( payload );
} 


```
Note that there are two classes of messages that Kibitz workers understand.  Collaboration messages are passed from worker to worker in order to perform some sort of job.  Notifications are used to communicate with external systems and to perform housekeeping tasks such as shutting down all workers or instructing workers to connect to other workers.


### [Collaboration](Collaboration.md) ###
### [Worker](Worker.md) ###