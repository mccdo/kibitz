## Messages ##

We define message structure below using [EBNF](http://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_Form).  The data interchange format itself will be JSON but we use EBNF to define the structure and content of the message.  We have two types of messages.  Collaboration messages that are messages exchanges between workers when performing a job, and notification messages that are for a variety of other management tasks such as discovery, system monitoring or letting external system know we've finished a job.

### Message Definition ###
```
digit = "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" ;

integer = digit, {digit} ;

version = integer ; 

ip address =  2 * digit, ".", 2 * digit, ".", 2 * digit, ".", 2 * digit ; 

worker type = ? a unique sequence of alphanumeric characters describe the business logic performed by the worker ? ;

timestamp = ? 64 bit integer count of microseconds since the epoch ? ; 

notification payload = [HeartbeatMessages heartbeat message], task complete message, job complete message, shut down message ; 

collaboration payload = collaboration id, task payload ; 

notification message = "notification", notification payload ; 

collaboration message = "collaboration", collaboration payload ; 

message = notification message | collaboration message ; 
```
### Message Class Layout ###

![https://kibitz.googlecode.com/git/doc/images/message_uml.png](https://kibitz.googlecode.com/git/doc/images/message_uml.png)