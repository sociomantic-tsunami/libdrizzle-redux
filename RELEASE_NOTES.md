Release Notes for v5.3.0
========================

Note: If you are upgrading from an older version, it is advisable to upgrade
incrementally, following the instructions in the previous versions' release
notes.

These notes are usually structured in 4 sections: **Migration Instructions**,
which are the mandatory steps a user must do to update to the new version,
**Deprecations**, which contains deprecated functions which are not recommended to
be used (and will be removed in the next major release) but will not break any
old code, **New Features** which are new features available in the new
version that users might find interesting; **Issues Fixed** which contains
bugfixes and improvements that don't extend the functionality.

Dependencies
------------

Dependency | Version
-----------|---------
gcc        | >=4.8.x
autoconf   | >=2.61
zlib1g-dev | >=0.13.x
libtool    | >=2.x

New Features
------------

* `drizzle_binlog_start()` has been modified, so binlog data can be read with a
  non-blocking connection. However, it requires the client to parse raw data
  from the connection file descriptor obtained through `drizzle_fd()`.
    ([#34](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/34),
     [#53](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/53))

Issues Fixed
------------

* Update hardcoded repo location
    ([#28](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull28))
* Fix version number in configure script
    ([#31](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull31))
* Update readme and doc
    ([#32](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull32))
* Change location of generated documentation to top-level folder `sphinx-build`
    ([#37](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/37))
* Restructure man page doc generation
    ([#38](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull38))
* Bugfix where calls to `drizzle_connect()` would always block regardless of the
  `non_blocking` option being set to true.
    ([#41](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/41))
* Optimization of `drizzle_field_buffer()` so an array of `drizzle_field_t`
  pointers are used to buffer fields
    ([#42](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/42))
* Minor fixes to .gitignore
    ([#44](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull44))
* Fixed integer underflow when processing large result sets using prepared
  statements
    ([#47](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/47))
* Use __func__ macro when logging functions
    ([#49](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull49))
* Update README with reasons for a sociomantic-tsunami fork
    ([#50](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull50))
* Whitespace fixes
    ([#51](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull51))
* Add continuous integration testing for clang compiler
    ([#57](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/57),
     [#58](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/58))
* Fix linter warnings in bootstrap.sh
    ([#61](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull61))
* Fix travis-ci build config for environment variables
    ([#63](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull63))
* Add travis-ci build status to README
    ([#65](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull65))
* Add unittest for event_watch_fn callback function
    ([#43](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/43),
     [#66](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/66),
     [#67](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/67))
     [#67](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull/70))
* Improve documentation for binlog code example
    ([#68](https://github.com/sociomantic-tsunami/libdrizzle-redux/pull68))
