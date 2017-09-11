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

   Creates a connection connection object. If a path beginning with ``/`` is
   given as the host the library will connect as a UDS socket. Otherwise a
   TCP/IP connection is made.

   .. note::
      a connection does not happen until the first query or an explicit :c:func:`drizzle_connect` call is made

   :param host: The socket path, hostname or IP of the server
   :param port: The port number of the server (if TCP/IP)
   :param user: The username of the server
   :param password: The password of the server
   :param db: The default DB to connect to on the server
   :param options: A pointer to a :c:type:`drizzle_options_st` created using :c:func:`drizzle_options_create` or :c:type:`NULL`
   :returns: A newly allocated and setup connection object

.. c:function:: int drizzle_fd(const drizzle_st *con)

   Get file descriptor for connection.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :returns: File descriptor of connection, or -1 if not active.

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

.. c:function:: void drizzle_set_event_watch_fn(drizzle_st *drizzle, drizzle_event_watch_fn *function, void *context)

   Set a custom I/O event watcher function for a drizzle structure

   :param drizzle: Drizzle structure previously initialized with :c:func:`drizzle_create`.
   :param function: Function to call when there is an I/O event, in the form of :c:func:`drizzle_event_watch_fn`
   :param context: Argument to pass into the callback function.

.. c:function:: drizzle_return_t drizzle_set_events(drizzle_st *con, short events)

   Set events to be watched for a connection.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param events: Bitfield of poll() events to watch.
   :returns: Standard drizzle return value.

.. c:function:: drizzle_return_t drizzle_set_revents(drizzle_st *con, short revents)

   Set events that are ready for a connection. This is used with the external
   event callbacks. See :c:func:`drizzle_set_event_watch_fn`.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param revents: Bitfield of poll() events that were detected.
   :returns: Standard drizzle return value.

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

.. c:function:: void drizzle_socket_set_options(drizzle_options_st *options, int wait_timeout, int keepidle, int keepcnt, int keepintvl)

   Sets several options for the socket connection

   :param options: An initialized options structure
   :param wait_timeout: The timeout (in seconds) for setsockopt calls with option values: SO_SNDTIMEO, SO_RCVTIMEO, SO_LINGER
   :param keepidle: The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes
   :param keepcnt: The maximum number of keepalive probes TCP should send before dropping the connection.
   :param keepintvl: The time (in seconds) between individual keepalive probes

.. c:function:: void drizzle_socket_set_option(drizzle_st *con, drizzle_socket_option_t option, int value)

   Sets the value of a socket option.

   .. note::
      The available options to set are:

      :py:const:`DRIZZLE_SOCKET_OPTION_TIMEOUT` : The timeout (in seconds) for setsockopt calls with option values: SO_SNDTIMEO, SO_RCVTIMEO, SO_LINGER

      :py:const:`DRIZZLE_SOCKET_OPTION_KEEPIDLE` : The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes

      :py:const:`DRIZZLE_SOCKET_OPTION_KEEPCNT` : The maximum number of keepalive probes TCP should send before dropping the connection.

      :py:const:`DRIZZLE_SOCKET_OPTION_KEEPINTVL` : The time (in seconds) between individual keepalive probes

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param option: the option to set the value for
   :param value: the value to set

.. c:function:: int drizzle_socket_get_option(drizzle_st *con, drizzle_socket_option_t option)

   Gets the value of a socket option. See :c:func:`drizzle_socket_set_options`
   for a description of the available options

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param option: option to get the value for
   :returns: The value of the option, or -1 if the specified option doesn't exist

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

   :param options: The options object to modify
   :param state: Set to true/false

.. c:function:: bool drizzle_options_get_auth_plugin(drizzle_options_st *options)

   Gets the auth plugin connect option

   :param options: The options object to get the value from
   :returns: The state of the auth plugin option

.. c:function:: void drizzle_options_set_socket_owner(drizzle_options_st *options, drizzle_socket_owner_t owner)

   Sets the owner of the socket connection

   :param options: The options object to modify
   :param owner: The owner of the socket connection

.. c:function:: drizzle_socket_owner_t drizzle_options_get_socket_owner(drizzle_options_st *options)

   Gets the owner of the socket connection

   :param options: The options object to get the value from
   :returns: The owner of the socket connection

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

.. c:function:: void *drizzle_context(const drizzle_st *con)

   Get application context pointer for a connection.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :returns: Application context with this connection.

.. c:function:: void drizzle_set_context(drizzle_st *con, void *context)

   Set application context pointer for a connection.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param context: Application context to use with this connection.

.. c:function:: void drizzle_set_context_free_fn(drizzle_st *con, drizzle_context_free_fn *function)

   Set callback function when the context pointer should be freed.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :param function: Function to call to clean up connection context.

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

.. c:function:: const unsigned char *drizzle_scramble(const drizzle_st *con)

   Get scramble buffer for a connection.

   :param con: Connection structure previously initialized with :c:func:`drizzle_create`.
   :returns: Scramble buffer for connection.

.. c:function:: drizzle_capabilities_t drizzle_capabilities(const drizzle_st *con)

   Gets the server capabilities for a connection

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
   :returns: A :c:type:`drizzle_return_t` status. :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_wait(drizzle_st *con)

   Wait for I/O on connections.

   :param drizzle: Drizzle structure previously initialized with :c:func:`drizzle_create`.
   :returns: Standard drizzle return value.

.. c:function:: drizzle_st *drizzle_ready(drizzle_st *con)

   Get next connection that is ready for I/O.

   :param drizzle: Drizzle structure previously initialized with :c:func:`drizzle_create`.
   :returns: Connection that is ready for I/O, or NULL if there are none.

.. c:function:: drizzle_return_t drizzle_close(drizzle_st *con)

   Gracefully disconnect from a server (leaves the connection object available
   for a reconnect)

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

.. c:function:: const char *drizzle_strerror(const drizzle_return_t ret)

   Get detailed error description

   :param ret: A libdrizzle return value
   :returns: description of libdrizzle error

Callback Functions
------------------

These are templates to be used when creating callback functions for the
Libdrizzle Redux library.

.. c:function:: void drizzle_log_fn(const char *log_buffer, drizzle_verbose_t verbose, void *context)

   The format of a callback function for log handling

   :param log_buffer: The log message passed to the function
   :param verbose: The verbosity level of the message
   :param context: A pointer to data set in :c:func:`drizzle_set_log_fn`

.. c:function:: drizzle_return_t drizzle_event_watch_fn(drizzle_st *con, short events, void *context)

   The format of a function to register or deregister interest in file descriptor
   events

   :param con: Connection that has changed the events it is interested in. Use :c:func:`drizzle_fd` to get the file descriptor.
   :param events: A bit mask of POLLIN | POLLOUT, specifying if the connection is waiting for read or write events.
   :param context: Application context pointer registered with :c:func:`drizzle_set_event_watch_fn`
   :returns: :py:const:`DRIZZLE_RETURN_OK` if successful.
