We use a specialized worker called a locator to help workers keep track of one another.  All workers send heartbeats to a locator.  The locator collects the heartbeats cataloging all the worker types and locations and rebroadcasts this information to all workers in a collaboration.  In other words each worker has an internal data structure that contains information on every other worker in it's system.  If a worker sends heartbeats to a locator and doesn't get heartbeats back, it switches to a secondary locator.  Having multiple locators avoids a single point of failure for this critical piece of functionality.