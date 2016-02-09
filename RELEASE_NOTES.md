Dependencies
============

Dependency | Version
-----------|---------
gcc        | >=4.8.x
autoconf   | >=2.61
zlib1g-dev | >=0.13.x
libtool    | >=2.x

New Features
============

* `libdrizzle-5.1/libdrizzle.h`

  Added support for custom callback function, to allow clients to be notified
  about file descriptor events. The new feature is available in the client API
  through the function `drizzle_set_event_watch_fn`

  Please refer to `docs/api/connection.rst` for documentation on
  `drizzle_set_event_watch_fn`
