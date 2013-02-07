Connection Functions
====================

Introduction
------------

This section outlines the connection functions

Structs
-------

.. c:type:: drizzle_st

   The internal drizzle connection object struct

.. c:type:: drizzle_options_st

   The internal structure containing connection options

Functions
---------

.. c:function:: drizzle_st* drizzle_create(const char *host, in_port_t port, const char *user, const char *password, const char *db, drizzle_options_st *options)

   Creates a connection connection object.  If a path beginning with ``/`` is given as the host the library will connect as a UDS socket.  Otherwise a TCP/IP conection is made.

   .. note::
      a connection does not happen until the first query or an explicit :c:func:`drizzle_connect` call is made

   :param host: The socket path, hostname or IP of the server
   :param port: The port number of the server (if TCP/IP)
   :param user: The username of the server
   :param password: The password of the server
   :param db: The default DB to connect to on the server
   :param options: A pointer to a :c:type:`drizzle_options_st` created using :c:func:`drizzle_options_create` or :c:type:`NULL`
   :returns: A newly allocated and setup connection object

.. c:function:: int drizzle_timeout(const drizzle_st *con)

   Gets the current connection timeout set in the connection object

   :param drizzle: A connection object
   :returns: The current timeout

.. c:function:: void drizzle_set_timeout(drizzle_st *con, int timeout)

   Sets the connection timeout for the connection object

   :param drizzle: A connection object
   :param timeout: The new timeout to set

.. c:function:: drizzle_verbose_t drizzle_verbose(const drizzle_st *con)

   Gets the verbosity level set in the connection object

   :param drizzle: A connection object
   :returns: The verbosity level from :c:type:`drizzle_verbose_t`

.. c:function:: void drizzle_set_verbose(drizzle_st *con, drizzle_verbose_t verbose)

   Sets the verbosity level for the connection object

   :param drizzle: A connection object
   :param verbose: The verbosity level from :c:type:`drizzle_verbose_t`

.. c:function:: void drizzle_set_log_fn(drizzle_st *con, drizzle_log_fn *function, void *context)

   Sets a callback function for log handling

   :param drizzle: A connection object
   :param function: The function to use in the format of :c:func:`drizzle_log_fn`
   :param context: A pointer to data to pass to the log function

.. c:function:: const char* drizzle_error(const drizzle_st *con)

   Get the last error from a connection

   :param con: A connection object
   :returns: A string containing the error message

.. c:function:: int drizzle_errno(const drizzle_st *con)

   Get the last OS error code from a connection

   :param con: A connection object
   :returns: The OS error code

.. c:function:: uint16_t drizzle_error_code(const drizzle_st *con)

   Gets the last error code from a connection

   :param con: A connection object
   :returns: The server error code

.. c:function:: const char* drizzle_sqlstate(const drizzle_st *con)

   Gets the last sqlstate from a connection

   :param con: A connection object
   :returns: A string containing the sqlstate

.. c:function:: drizzle_options_st *drizzle_options_create(void)

   Create a new connection options object

   :returns: The new connection options object

.. c:function:: void drizzle_options_destroy(drizzle_options_st *options)

   Destroys a connection options object

   :param options: The options object to be destroyed

.. c:function:: void drizzle_options_set_non_blocking(drizzle_options_st *options, bool state)

   Sets/unsets non-blocking connect option

   :param options: The options object to modify
   :param state: Set option to true/false

.. c:function:: bool drizzle_options_get_non_blocking(drizzle_options_st *options)

   Gets the non-blocking connect option

   :param options: The options object to get the value from
   :returns: The state of the non-blocking option

.. c:function:: void drizzle_options_set_raw_scramble(drizzle_options_st *options, bool state)

   Sets/unsets the raw scramble connect option

   :param options: The options object to modify
   :param state: Set to true/false

.. c:function:: bool drizzle_options_get_raw_scramble(drizzle_options_st *options)

   Gets the raw scramble connect option

   :param options: The options object to get the value from
   :returns: The state of the raw scramble option

.. c:function:: void drizzle_options_set_found_rows(drizzle_options_st *options, bool state)

   Sets/unsets the found rows connect option

   :param options: The options object to modify
   :param state: Set to true/false

.. c:function:: bool drizzle_options_get_found_rows(drizzle_options_st *options)

   Gets the found rows connect option

   :param options: The options object to get the value from
   :returns: The state of the found rows option

.. c:function:: void drizzle_options_set_interactive(drizzle_options_st *options, bool state)

   Sets/unsets the interactive connect option

   :param options: The options object to modify
   :param state: Set to true/false

.. c:function:: bool drizzle_options_get_interactive(drizzle_options_st *option)

   Gets the interactive connect option

   :param options: The options object to get the value from
   :returns: The state of the interactive option

.. c:function:: void drizzle_options_set_multi_statements(drizzle_options_st *options, bool state)

   Sets/unsets the multi-statements connect option

   :param options: The options object to modify
   :parma state: Set to true/false

.. c:function:: bool drizzle_options_get_multi_statements(drizzle_options_st *options)

   Gets the multi-statements connect option

   :param options: The options object to get the value from
   :returns: The state of the multi-statements option

.. c:function:: void drizzle_options_set_auth_plugin(drizzle_options_st *options, bool state)

   Sets/unsets the auth plugin connect option

   :param options: The optoins object to modify
   :param state: Set to true/false

.. c:function:: bool drizzle_options_get_auth_plugin(drizzle_options_st *options)

   Gets the auth plugin connect option

   :param options: The options object to get the value from
   :returns: The state of the auth plugin option
   
.. c:function:: const char* drizzle_host(const drizzle_st *con)

   Gets the host name from a TCP/IP connection

   :param con: A connection object
   :returns: A string containing the host name or NULL for a UDS connection

.. c:function:: in_port_t drizzle_port(const drizzle_st *con)

   Gets the port number from a TCP/IP connection

   :param con: A connection object
   :returns: The port number or 0 for a UDS connection

.. c:function:: const char* drizzle_user(const drizzle_st *con)

   Gets the user name used at connection time

   :param con: A connection object
   :returns: A string containing the user name

.. c:function:: const char* drizzle_db(const drizzle_st *con)

   Gets the default database used at connection time

   :param con: A connection object
   :returns: A string containing the DB name

.. c:function:: uint8_t drizzle_protocol_version(const drizzle_st *con)

   Gets the protocol version used for a connection

   :param con: A connection object
   :returns: The protocol version

.. c:function:: const char* drizzle_server_version(const drizzle_st *con)

   Gets the server version string for a connection

   :param con: A connection object
   :returns: A string containing the server version

.. c:function:: uint32_t drizzle_server_version_number(const drizzle_st *con)

   Gets the server version number for a connection

   :param con: A connection object
   :returns: An integer containing the server version number

.. c:function:: uint32_t drizzle_thread_id(const drizzle_st *con)

   Gets the server thread ID for a connection

   :param con: A connection object
   :returns: The server thread ID

.. c:function:: drizzle_capabilities_t drizzle_capabilities(const drizzle_st *con)

   Gets the server capabilites for a connection

   :param con: A connection object
   :returns: A bit field of capabilities

.. c:function:: drizzle_charset_t drizzle_charset(const drizzle_st *con)

   Gets the character set ID for the connection

   :param con: A connection object
   :returns: The character set used

.. c:function:: drizzle_status_t drizzle_status(const drizzle_st *con)

   Gets the status of the connection

   :param con: A connection object
   :returns: The status of the connection

.. c:function:: uint32_t drizzle_max_packet_size(const drizzle_st *con)

   Gets the max packet size for a connection

   :param con: A connection object
   :returns: The max packet size for the connection

.. c:function:: drizzle_return_t drizzle_connect(drizzle_st *con)

   Open connection to the specified server

   :param con: A connection object
   :returns: A :c:type:`drizzle_return_t` status.  :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_close(drizzle_st *con)

   Gracefully disconnect from a server (leaves the connection object available for a reconnect

   :param con: A connection object
   :returns: A :c:type:`drizzle_return_t` response for the quit command sent to the server

.. c:function:: drizzle_return_t drizzle_quit(drizzle_st *con)

   Gracefully disconnect from a server and free the connection object

   :param con: A connection object
   :returns: A :c:type:`drizzle_return_t` response for the quit command sent to the server

.. c:function:: drizzle_return_t drizzle_select_db(drizzle_st *con, const char *db)

   Change the current default database

   :param con: A connection object
   :param db: The new default database
   :returns: A :c:type:`drizzle_return_t` response

.. c:function:: drizzle_result_st* drizzle_shutdown(drizzle_st *con, drizzle_return_t *ret_ptr)

   Send a shutdown command to the server

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_kill(drizzle_st *con, uint32_t connection_id, drizzle_return_t *ret_ptr)

   Sends a query kill command to the server

   :param con: A connection object
   :param connection_id: The connection ID to kill a query from
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_ping(drizzle_st *con, drizzle_return_t *ret_ptr)

   Sends a ping to the server

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object


Callback Functions
------------------

These are templates to be used when creating callback functions for the
Libdrizzle Redux library.

.. c:function:: void drizzle_log_fn(const char *log_buffer, drizzle_verbose_t verbose, void *context)

   The format of a callback function for log handling

   :param log_buffer: The log message passed to the function
   :param verbose: The verbosity level of the message
   :param context: A pointer to data set in :c:func:`drizzle_set_log_fn`

