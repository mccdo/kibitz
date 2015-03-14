### Collaboration Messages and Redundancy ###

Collaboration messages may have any number of task payloads which vary based on data being exchanged between workers, but all share a common collaboration id.  This id is a [globally unique identifier](http://en.wikipedia.org/wiki/Globally_unique_identifier) that originates at the head node worker that initiates a job and follows different task payload messages as we traverse a directed graph that defines a [collaboration](Collaboration.md). This allows us to have redundant workers perform the same task.  Of course if we do this a subsequent worker may receive two messages for the same job.  To avoid this, a worker only takes action when it sees a unique collaboration id.  If a worker sees duplicate collaboration id's it knows it has already performed a task associated with a particular job and discards the message. In the diagram below both worker B and worker B' are redundant. This means D and E receive duplicate task payloads, but since the duplicates have the same collaboration id, D and E only process a single message and discard the other.

![https://kibitz.googlecode.com/git/doc/images/collaboration_id.png](https://kibitz.googlecode.com/git/doc/images/collaboration_id.png)

### Collaboration Setup Message ###

This is a special type of collaboration message that is that is used by the [scheduler](SchedulerWorker.md) to set up a collaboration.  This must be done prior to executing a job.
```
worker id = ? globally unique identifier ? ;

worker = [worker id], worker type;
 
task step = worker, { task step } ;

payload = task step ;

collaboration payload = "setup", version, payload ;
```
[Example JSON](https://gist.github.com/3304918)