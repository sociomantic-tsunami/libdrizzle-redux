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

.. c:function:: drizzle_result_st* drizzle_start_binlog(drizzle_st *con, uint32_t server_id, const char *file, uint32_t start_position, drizzle_return_t *ret_ptr)

   Start the binlog transaction.  Set the server_id to 0 to disconnect
   automatically at the end of the last log.

   :param con: Drizzle connection object
   :param server_id: A unique server ID (or 0) to connect to the MySQL server with
   :param file: The start binlog file, can be empty to start at the first known file
   :param start_position: The position of the binlog file to start at, a value of less than 4 is set to 4 due to the binlog header taking the first 4 bytes
   :param ret_ptr: A pointer to a Drizzle return type.  :py:const:`DRIZZLE_RETURN_OK` upon success.
   :returns: A Drizzle result object or NULL on error

.. c:function:: drizzle_return_t drizzle_binlog_get_next_event(drizzle_result_st *result)

   Get the next event from the binlog stream.

   :param result: The result set for the binlog stream
   :returns: :py:const:`DRIZZLE_RETURN_OK` upon success or :py:const:`DRIZZLE_RETURN_EOF` if there is no more data

.. c:function:: uint32_t drizzle_binlog_event_timestamp(drizzle_result_st *result)

   Get the timestamp for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The timestamp for the binlog event

.. c:function:: drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_result_st *result)

   Get the event type for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The timestamp for the binlog event

.. c:function:: uint32_t drizzle_binlog_event_server_id(drizzle_result_st *result)

   Get the server_id for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The server_id for the binlog event

.. c:function:: uint32_t drizzle_binlog_event_length(drizzle_result_st *result)

   Get the length of the event data for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The event data length

.. c:function:: uint32_t drizzle_binlog_event_next_pos(drizzle_result_st *result)

   Get the next event position from the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The next event position

.. c:function:: uint16_t drizzle_binlog_event_flags(drizzle_result_st *result)

   Get the flags for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The event flags

.. c:function:: uint16_t drizzle_binlog_event_extra_flags(drizzle_result_st *result)

   Get the extra flags for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The extra event flags

.. c:function:: const unsigned char* drizzle_binlog_event_data(drizzle_result_st *result)

   Get the event data for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: A pointer to the event data

.. c:function:: const unsigned char* drizzle_binlog_event_raw_data(drizzle_result_st *result)

   Get the raw event data (including header) for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: A pointer to the raw event data

.. c:function:: uint32_t drizzle_binlog_event_raw_length(drizzle_result_st *result)

   Get the length of the raw event data (including header) for the event received using :c:func:`drizzle_binlog_get_next_event`

   :param result: The result set for the binlog stream
   :returns: The length of the raw event data

