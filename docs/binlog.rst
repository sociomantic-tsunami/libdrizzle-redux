Binlog Functions
================

Introduction
------------

Libdrizzle Redux contains functions which give it the capabilities to connect
as a MySQL slave or a mysqlbinlog type client and retrieve the events.

.. warning::
   You should start a binlog retrieval on a new connection only.  Running on a
   connection that has already executed queries has an undefined (usually bad)
   behaviour.

Functions
---------

.. c:function:: drizzle_result_st* drizzle_start_binlog(drizzle_con_st *con, uint32_t server_id, const char *file, uint32_t start_position, drizzle_return_t *ret_ptr)

   Start the binlog transaction.  Set the server_id to 0 to disconnect
   automatically at the end of the last log.

   :param con: Drizzle connection object
   :param server_id: A unique server ID (or 0) to connect to the MySQL server with
   :param file: The start binlog file, can be empty to start at the first known file
   :param start_position: The position of the binlog file to start at, a value of less than 4 is set to 4 due to the binlog header taking the first 4 bytes
   :param ret_ptr: A pointer to a Drizzle return type.  :py:const:`DRIZZLE_RETURN_OK` upon success.
   :returns: A Drizzle result object or NULL on error

.. c:function:: drizzle_binlog_st* drizzle_binlog_get_event(drizzle_result_st *result, drizzle_return_t *ret_ptr)

   Get a single event from the binlog stream.

   :param result: The result set for the binlog stream
   :param ret_ptr: A pointer to a Drizzle return type.  :py:const:`DRIZZLE_RETURN_OK` upon success or :py:const:`DRIZZLE_RETURN_EOF` if there is no more data
   :returns: A Binlog event struct
