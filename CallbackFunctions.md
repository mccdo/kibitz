Callback functions are used to implement operations on messages that are sent from other workers or handle notifications.

### Handle messages from other workers ###
```
typedef void (*collaboration_callback)( const collaboration_messages_t& messages );

void set_in_message_handler( collaboration_callback cbfn );

```
### Handle notifications that start job ###

Jobs are kicked off by sending a notification to one or more workers.  These initial worker nodes don't have any in edges so the notification is used to get things going.  Typically one would supply a handler that retrieves data needed to begin the job from an external data source and then call send\_out\_message to transmit collaboration messages to child workers.

```
typedef void (*initialization_callback)( void );

void set_initialization_notification_handler( initialization_callback cbfn );
```