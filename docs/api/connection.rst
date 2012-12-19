Connection Functions
====================

Introduction
------------

This section outlines the connection functions

Functions
---------

.. c:function:: drizzle_con_st* drizzle_con_add_tcp(drizzle_st *drizzle, const char *host, in_port_t port, const char *user, const char *password, const char *db, drizzle_con_options_t options)

   Allocates and adds a TCP/IP connection to a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param host: The hostname or IP of the server
   :param port: The port number of the server
   :param user: The username of the server
   :param password: The password of the server
   :param db: The default DB to connect to on the server
   :param options: The connection options to set
   :returns: A newly allocated and setup connection object

.. c:function:: drizzle_con_st* drizzle_con_add_uds(drizzle_st *drizzle, const char *uds, const char *user, const char *password, const char *db, drizzle_con_options_t options)

   Allocates and adds a Unix Domain Socket connection to a Drizzle object

   :param drizzle: A Drizzle object allocated with :c:func:`drizzle_create`
   :param uds: The path of the UDS file
   :param user: The username of the server
   :param password: The password of the server
   :param db: The default DB to connect to on the server
   :param options: The connection options to set
   :returns: A newly allocated and setup connection object

.. c:function:: drizzle_st* drizzle_con_drizzle(const drizzle_con_st *con)

   Gets the Drizzle object from the connection object

   :param con: A connection object
   :returns: A Drizzle object the connection object is assigned to

.. c:function:: const char* drizzle_con_error(const drizzle_con_st *con)

   Get the last error from a connection

   .. note::
      This actually gets the error from the Drizzle object so will be
      overwritten with a new error if another connection in the same group
      also errors

   :param con: A connection object
   :returns: A string containing the error message

.. c:function:: int drizzle_con_errno(const drizzle_con_st *con)

   Get the last OS error code from a connection

   .. note::
      This actually gets the error from the Drizzle object so will be
      overwritten with a new error if another connection in the same group
      also errors

   :param con: A connection object
   :returns: The OS error code

.. c:function:: uint16_t drizzle_con_error_code(const drizzle_con_st *con)

   Gets the last error code from a connection

   .. note::
      This actually gets the error from the Drizzle object so will be
      overwritten with a new error if another connection in the same group
      also errors

   :param con: A connection object
   :returns: The server error code

.. c:function:: const char* drizzle_con_sqlstate(const drizzle_con_st *con)

   Gets the last sqlstate from a connection

   .. note::
      This actually gets the error from the Drizzle object so will be
      overwritten with a new error if another connection in the same group
      also errors

   :param con: A connection object
   :returns: A string containing the sqlstate

.. c:function:: drizzle_con_options_t drizzle_con_options(const drizzle_con_st *con)

   Gets the connection options

   :param con: A connection object
   :returns: The options for the connection

.. c:function:: void drizzle_con_set_options(drizzle_con_st *con, drizzle_con_options_t options)

   Sets the connection options

   :param con: A connection object
   :param options: A bit field of the :c:type:`drizzle_con_options_t` options

.. c:function:: void drizzle_con_add_options(drizzle_con_st *con, drizzle_con_options_t options)

   Add connection options

   :param con: A connection object
   :param options: A bit field of the :c:type:`drizzle_con_options_t` options

.. c:function:: void drizzle_con_remove_options(drizzle_con_st *con, drizzle_con_options_t options)

   Removes connection options

   :param con: A connection object
   :param options: A bit field of the :c:type:`drizzle_con_options_t` options

.. c:function:: const char* drizzle_con_host(const drizzle_con_st *con)

   Gets the host name from a TCP/IP connection

   :param con: A connection object
   :returns: A string containing the host name or NULL for a UDS connection

.. c:function:: in_port_t drizzle_con_port(const drizzle_con_st *con)

   Gets the port number from a TCP/IP connection

   :param con: A connection object
   :returns: The port number or 0 for a UDS connection

.. c:function:: const char* drizzle_con_user(const drizzle_con_st *con)

   Gets the user name used at connection time

   :param con: A connection object
   :returns: A string containing the user name

.. c:function:: const char* drizzle_con_db(const drizzle_con_st *con)

   Gets the default database used at connection time

   :param con: A connection object
   :returns: A string containing the DB name

.. c:function:: uint8_t drizzle_con_protocol_version(const drizzle_con_st *con)

   Gets the protocol version used for a connection

   :param con: A connection object
   :returns: The protocol version

.. c:function:: const char* drizzle_con_server_version(const drizzle_con_st *con)

   Gets the server version string for a connection

   :param con: A connection object
   :returns: A string containing the server version

.. c:function:: uint32_t drizzle_con_server_version_number(const drizzle_con_st *con)

   Gets the server version number for a connection

   :param con: A connection object
   :returns: An integer containing the server version number

.. c:function:: uint32_t drizzle_con_thread_id(const drizzle_con_st *con)

   Gets the server thread ID for a connection

   :param con: A connection object
   :returns: The server thread ID

.. c:function:: drizzle_capabilities_t drizzle_con_capabilities(const drizzle_con_st *con)

   Gets the server capabilites for a connection

   :param con: A connection object
   :returns: A bit field of capabilities

.. c:function:: drizzle_charset_t drizzle_con_charset(const drizzle_con_st *con)

   Gets the character set ID for the connection

   :param con: A connection object
   :returns: The character set used

.. c:function:: drizzle_con_status_t drizzle_con_status(const drizzle_con_st *con)

   Gets the status of the connection

   :param con: A connection object
   :returns: The status of the connection

.. c:function:: uint32_t drizzle_con_max_packet_size(const drizzle_con_st *con)

   Gets the max packet size for a connection

   :param con: A connection object
   :returns: The max packet size for the connection

.. c:function:: drizzle_return_t drizzle_con_connect(drizzle_con_st *con)

   Open connection to the specified server

   :param con: A connection object
   :returns: A :c:type:`drizzle_return_t` status.  :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_con_quit(drizzle_con_st *con)

   Gracefully disconnect from a server and free the connection object

   :param con: A connection object
   :returns: A :c:type:`drizzle_return_t` response for the quit command sent to the server

.. c:function:: drizzle_result_st* drizzle_con_select_db(drizzle_con_st *con, const char *db, drizzle_return_t *ret_ptr)

   Change the current default database

   :param con: A connection object
   :param db: The new default database
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_con_shutdown(drizzle_con_st *con, drizzle_return_t *ret_ptr)

   Send a shutdown command to the server

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_con_kill(drizzle_con_st *con, uint32_t connection_id, drizzle_return_t *ret_ptr)

   Sends a query kill command to the server

   :param con: A connection object
   :param connection_id: The connection ID to kill a query from
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_con_ping(drizzle_con_st *con, drizzle_return_t *ret_ptr)

   Sends a ping to the server

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object


