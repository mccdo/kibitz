## Collaboration ##

A collaboration is a group of workers interacting to perform a job.  A collaboration is implemented as a directed graph of [workers](Worker.md).  Each worker represents a node in the graph.  Edges in the graph represent paths that messages traverse from predecessor worker to successor workers.  Edges are implemented as 0MQ pub sub sockets implying one or more workers may subscribe to receive a result message a direct predecessor worker produces. Workers may have one or more in edges and one or more out edges except in the case of head workers or terminal workers.

The head or initial worker has no in edges the starts the job when it receives a notification message.  It then performs its work and publishes a message containing a result to its out edges.  Workers with in edges begin work when all that worker's in edges are signalled (have a message). Then, when that worker's operation is complete, it publishes a message to it's out edges and so on until we encounter a terminal worker.  The terminal worker does not have out edges and publishes a notification indicating completion. The notification message transmits results of the collaboration to any subscribing systems that are interested in it (graphics).

![https://kibitz.googlecode.com/git/doc/images/Collaboration.png](https://kibitz.googlecode.com/git/doc/images/Collaboration.png)


