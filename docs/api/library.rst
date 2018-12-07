Library Functions
=================

Introduction
------------

This section outlines the functions related to the Libdrizzle Redux library

Functions
---------

.. c:function:: void drizzle_library_init(void)

   Setup of the SSL and Windows connection libraries.  Should be run before any
   Libdrizzle Redux function.  Only required if using SSL or Windows.

.. c:function:: void drizzle_library_deinit(void)

   Deinitialize the library.  Only required for Windows

.. c:function:: const char* drizzle_version(void)

   Gives the version string for the Libdrizzle Redux library

   :returns: A string of the library version

.. c:function:: const char* drizzle_bugreport(void)

   Gives the URL for reporting library bugs

   :returns: A string containing the bug report URL

.. c:function:: const char* drizzle_verbose_name(const drizzle_verbose_t verbose)

   Gives the verbosity name for a given verbosity type

   :returns: A string containing the verbosity name
