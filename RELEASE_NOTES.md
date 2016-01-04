Dependencies
============

Dependency | Version
-----------|---------
gcc        | >=4.8.x
automake   | >=v1.12
CLooG      | >=v0.18.x


New Features
============

* `libdrizzle-5.1/libdrizzle.h`

  Added support for custom callback function, to allow clients to be notified
  about file descriptor events. The new feature is available in the client API
  through the function `drizzle_set_event_watch_fn`

  Please refer to `docs/api/connection.rst` for documentation on
  `drizzle_set_event_watch_fn`
