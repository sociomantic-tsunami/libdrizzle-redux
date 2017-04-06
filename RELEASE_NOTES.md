Dependencies
============

Dependency | Version
-----------|---------
gcc        | >=4.8.x
autoconf   | >=2.61
zlib1g-dev | >=0.13.x
libtool    | >=2.x
openssl<sup>1</sup> | >=v1.x

<sup>1</sup> openssl is needed if libdrizzle-redux is compiled with support for
SSL connections.

New project layout
==================

A more general project layout has been adopted such that source files are now
located in the folder `src` and headers for the public interface are in `include/libdrizzle-redux`.

Migration Instructions
======================

- header file include path

    Projects using **libdrizzle-redux** header files should change `#include`
    directives from `#include <libdrizzle-5.1/[HEADERFILE].h>` to
    `#include <libdrizzle-redux/[HEADERFILE].h>`.

- `binlog.h`

    The signature of the function `drizzle_binlog_get_filename` changed from

    `void drizzle_binlog_get_filename(drizzle_st* con, char** filename, int file_index)`

    to

    `drizzle_return_t drizzle_binlog_get_filename(drizzle_st* con, char** filename,
    uint32_t* end_position, int file_index)`

- `constants.h`

    The definition of the callback function for log handling `drizzle_log_fn`
    has changed from

    `typedef void (drizzle_log_fn)(const char *line, drizzle_verbose_t verbose,
    void *context);`

    to

    `typedef void (drizzle_log_fn)(const char *file, uint line, const char *func,
    const char *msg, drizzle_verbose_t verbose, void *context)`

- `drizzle.h`

    In the function `drizzle_set_log_fn` the base type of second parameter `function`
    has been changed from

    `void(*) (char const*, enum drizzle_verbose_t, void*)`

    to

    `void(*) (char const*, uint, char const*, char const*, enum drizzle_verbose_t, void*)`

New Features
============

- `drizzle_binlog_get_filename`

    The function now allows the client to get the size of the binlog file in bytes.
    The retrieved filesize can be used to read from the end of the binlog file when
    calling `drizzle_binlog_start`.

    The return type has been changed to `drizzle_return_t`. If no errors occurred
    **`DRIZZLE_RETURN_OK`** is returned, otherwise the underlying error message
    is accessible by calling `drizzle_error`

- `drizzle_log_fn`

    The signature of the client callback function for log handling has been
    extended with parameters for **filename**, **linenumber** and **function-name**

Issues fixed
============

- Minor documentation fixes
- Improved documentation for ABI versioning
