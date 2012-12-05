Library Functions
=================

Introduction
------------

This section outlines the functions related to the Libdrizzle Redux library

Functions
---------

.. c:function:: void drizzle_library_init(void)

   Setup of the SSL library.  Should be run before any Libdrizzle Redux
   function.  Only required if using SSL.

.. c:function:: const char* drizzle_version(void)

   Gives the version string for the Libdrizzle Redux library

   :returns: A string of the library version

.. c:function:: const char* drizzle_bugreport(void)

   Gives the URL for reporting library bugs

   :returns: A string containing the bug report URL

.. c:function:: const char* drizzle_verbose_name(drizzle_verbose_t verbose)

   Gives the verbosity name for a given verbosity type

   :returns: A string containing the verbosity name

.. c:function:: drizzle_st* drizzle_create(void)

   Create a new Drizzle object which contains one or more connections

   :returns: An allocated Drizzle object or NULL upon failure

.. c:function:: void drizzle_free(drizzle_st *drizzle)

   Free an allocated Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`

.. c:function:: const char* drizzle_error(const drizzle_st *drizzle)

   Gets the last error on the Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: A string containing the last error message

.. c:function:: int drizzle_errno(const drizzle_st *drizzle)

   Gets the last OS error code for a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: The last OS error code

.. c:function:: uint16_t drizzle_error_code(const drizzle_st *drizzle)

   Gets the last server error code

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: The last server error code

.. c:function:: const char* drizzle_sqlstate(const drizzle_st *drizzle)

   Gets the last server sqlstate

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: A string of the last error state

.. c:function:: void drizzle_set_options(drizzle_st *drizzle, drizzle_options_t options)

   Sets the options for a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param options: A bit field of the :c:type:`drizzle_options_t` options

.. c:function:: void drizzle_add_options(drizzle_st *drizzle, drizzle_options_t options)

   Adds options for a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param options: A bit field of the :c:type:`drizzle_options_t` options

.. c:function:: void drizzle_remove_options(drizzle_st *drizzle, drizzle_options_t options)

   Removes options for a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param options: A bit field of the :c:type:`drizzle_options_t` options

.. c:function:: int drizzle_timeout(const drizzle_st *drizzle)

   Gets the current connection timeout set in the Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: The current timeout

.. c:function:: void drizzle_set_timeout(drizzle_st *drizzle, int timeout)

   Sets the connection timeout for the Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param int: The new timeout to set

.. c:function:: drizzle_verbose_t drizzle_verbose(const drizzle_st *drizzle)

   Gets the verbosity level set in the Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :returns: The verbosity level from :c:type:`drizzle_verbose_t`

.. c:function:: void drizzle_set_verbose(drizzle_st *drizzle, drizzle_verbose_t verbose)

   Sets the verbosity level for the Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param verbose: The verbosity level from :c:type:`drizzle_verbose_t`

.. c:function:: void drizzle_set_log_fn(drizzle_st *drizzle, drizzle_log_fn *function, void *context)

   Sets a callback function for log handling

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param function: The function to use in the format of :c:func:`drizzle_log_fn`
   :param context: A pointer to data to pass to the log function

Callback Functions
------------------

These are templates to be used when creating callback functions for the
Libdrizzle Redux library.

.. c:function:: void drizzle_log_fn(const char *log_buffer, drizzle_verbose_t verbose, void *context)

   The format of a callback function for log handling

   :param log_buffer: The log message passed to the function
   :param verbose: The verbosity level of the message
   :param context: A pointer to data set in :c:func:`drizzle_set_log_fn`
