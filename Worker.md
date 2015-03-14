## Worker ##

A worker is a container that implements a discrete unit of business logic. To host business logic in a worker container, a developer would include the worker shared library in their project and implement a set of call back functions to handle and send messages.  All workers share core functions including:
  * Broadcasting existence to other workers
  * Keep track of other workers that are broadcasting their existence
  * Sending notifications (based on something it has done or something it needs)
  * Receiving notifications and take action on notifications it cares about
  * Participating in [collaborations](Collaboration.md) with other workers to accomplish tasks

All workers provide that following set of services:
  * Daemonization and lifetime management
  * Logging
  * Heartbeat Broadcast
  * Listening to heartbeats and, based on heartbeats maintaining metadata about other workers and what they do
  * Message handling
  * Message routing

Each worker is characterized by the specialized work it does, typically some discrete implementation of business logic.  We consider this the worker type.  We want multiple workers of each type to be available to improve the resiliency of the overall system.

Workers are always running so we can avoid timing problems and complexity involved with spinning up a process to perform tasks.

A worker can perform its tasks concurrently.