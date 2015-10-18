I live-coded this library on https://www.livecoding.tv/yoones/

Libanio
====


Libanio is a TCP library to make your life easy when writing a server.

**Language:** C
**Licence:** GNU/GPL
**Github:** (https://github.com/yoones/)

Features:
- **Multiplexing:** all file descriptors are monitored using epoll().
- **Events-based:** you set a callback for the events you want to watch (when accepting a new client, when receiving data, when a client disconnects, etc.), they'll be called only when needed.
- **Multithreading:** the monitor that waits for events runs in a separate thread and has a thread pool of workers that handle events as they occur.
- **I/O buffering:** every file descriptor has its own read and write buffer. If data is read but is not to be consumed yet, it's stacked. If data is to be sent but non-blocking write operation is not possible at the moment, it is stacked and write operation will be tried later.
- **Different reading modes:** depending on the type of protocol you want to implement, 3 reading modes are available:
  - *STREAM:* consume data as it comes.
    - *BLOCK:* wait until a complete block of predefined size is available before calling the on_read callback. Useful for instance in VoIP projects.
      - *LINE:* wait until a complete line ending with predefined delimiter is available before calling the on_read callback. Useful when writing text-based protocols.

> If you want to see me code something in particular, tweet me **@yoones_s** with your request and I'll see what I can do :)
