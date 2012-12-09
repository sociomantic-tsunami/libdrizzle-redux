Structs
=======

Introduction
------------

This is a breakdown of structs provided with Libdrizzle Redux.

.. note::
   All structs are also typedef'd so you do not need to use the 'struct'
   keyword when using them.

Binlog
------

.. c:type:: drizzle_binlog_st

   The public struct containing a binlog event

   .. c:member:: uint32_t timestamp

      A timestamp for the log event

   .. c:member:: drizzle_binlog_event_types_t type

      The type of event for this struct

   .. c:member:: uint32_t server_id

      The server ID the binlog event was executed on

   .. c:member:: uint32_t length

      The length of the binlog event data

   .. c:member:: uint32_t next_pos

      The position of the next event in the binlog

   .. c:member:: uint16_t flags

      The flags for the binlog event

   .. c:member:: uint16_t extra_flags

      More flags for the binlog event

   .. c:member:: uint8_t* data

      A pointer to the data, data needs to be copied before the next event is
      retrieved

Query
-----

.. c:type:: drizzle_query_st

   The internal query object struct

.. c:type:: drizzle_result_st

   The internal result object struct

.. c:type:: drizzle_column_st

   The internal column object struct

Connection
----------

.. c:type:: drizzle_st

   The internal drizzle object struct

.. c:type:: drizzle_con_st

   The internal drizzle connection object struct
