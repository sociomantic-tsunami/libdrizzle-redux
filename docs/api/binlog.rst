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

The binlog functions use a callback API so that a function in the user
application will be called whenever there is a new event to retrieve.

Structs
-------

.. c:type:: drizzle_binlog_st

   The internal struct containing the binlog stream information

.. c:type:: drizzle_binlog_event_st

   The internal struct containing the binlog event header and data

Callback Functions
------------------

There are two callback functions.  The first is called whenever a new event is
available to retrieve.  The second is triggered whenever an error (or EOF)
occurs.

.. c:function:: void (drizzle_binlog_fn)(drizzle_binlog_event_st *event, void *context)

   This defines the function that will be supplied to accept binlog events

   .. warning::
      Event data needs to be copied/processed before exiting the function, it
      will be erased before the next callback.

   :param event: A pointer to the event struct
   :param context: A user defined pointer supplied in :c:func:`drizzle_binlog_init`

.. c:function:: void (drizzle_binlog_error_fn)(drizzle_return_t error, drizzle_st *con, void *context)

   This defines the function that will be supplied to accept binlog errors

   :param error: The :c:type:`drizzle_return_t` for the error (or :py:const:`DRIZZLE_RETURN_EOF` when all events have been retrieved
   :param con: The connection object the error occured on
   :param context: A user defined pointer supplied in :c:func:`drizzle_binlog_init`

Functions
---------

.. c:function:: drizzle_binlog_st *drizzle_binlog_init(drizzle_st *con, drizzle_binlog_fn *binlog_fn, drizzle_binlog_error_fn *error_fn, void *context, bool verify_checksums)

   Initializes a binlog object for the connection and sets the event callback
   functions

   :param con: The connection the binlog retrieval will be on
   :param binlog_fn: The function callback defined in :c:func:`(drizzle_binlog_fn)`
   :param error_fn: The function callback defined in :c:func:`(drizzle_binlog_error_fn)`
   :param context: A pointer to user data which will be used for the callback functions
   :param verify_checksums: Set to true if MySQL 5.6 and higher checksums should be verified

.. c:function:: void drizzle_binlog_free(drizzle_binlog_st *binlog)

   Frees a binlog object created with :c:func:`drizzle_binlog_init`

   :param binlog: The binlog object to be freed

.. c:function:: drizzle_return_t drizzle_binlog_start(drizzle_binlog_st *binlog, uint32_t server_id, const char *file, uint32_t start_position)

   Start the binlog transaction.  Set the server_id to 0 to disconnect
   automatically at the end of the last log.

   :param binlog: A binlog object created using :c:func:`drizzle_binlog_init`
   :param server_id: A unique server ID (or 0) to connect to the MySQL server with
   :param file: The start binlog file, can be empty to start at the first known file
   :param start_position: The position of the binlog file to start at, a value of less than 4 is set to 4 due to the binlog header taking the first 4 bytes
   :returns: A Drizzle return type.  :py:const:`DRIZZLE_RETURN_OK` upon success.

.. c:function:: uint32_t drizzle_binlog_event_timestamp(drizzle_binlog_event_st *event)

   Get the timestamp for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The timestamp for the binlog event

.. c:function:: drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_binlog_event_st *event)

   Get the event type for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The timestamp for the binlog event

.. c:function:: uint32_t drizzle_binlog_event_server_id(drizzle_binlog_event_st *event)

   Get the server_id for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The server_id for the binlog event

.. c:function:: uint32_t drizzle_binlog_event_length(drizzle_binlog_event_st *event)

   Get the length of the event data received by the event callback

   :param event: The event from binlog stream
   :returns: The event data length

.. c:function:: uint32_t drizzle_binlog_event_next_pos(drizzle_binlog_event_st *event)

   Get the next event position from the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The next event position

.. c:function:: uint16_t drizzle_binlog_event_flags(drizzle_binlog_event_st *event)

   Get the flags for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The event flags

.. c:function:: uint16_t drizzle_binlog_event_extra_flags(drizzle_binlog_event_st *event)

   Get the extra flags for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The extra event flags

.. c:function:: const unsigned char* drizzle_binlog_event_data(drizzle_binlog_event_st *event)

   Get the event data for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: A pointer to the event data

.. c:function:: const unsigned char* drizzle_binlog_event_raw_data(drizzle_binlog_event_st *event)

   Get the raw event data (including header) for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: A pointer to the raw event data

.. c:function:: uint32_t drizzle_binlog_event_raw_length(drizzle_binlog_event_st *event)

   Get the length of the raw event data (including header) for the event received by the event callback

   :param event: The event from the binlog stream
   :returns: The length of the raw event data

