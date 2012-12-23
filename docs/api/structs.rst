Structs
=======

Introduction
------------

This is a breakdown of structs provided with Libdrizzle Redux.

.. note::
   All structs are also typedef'd so you do not need to use the 'struct'
   keyword when using them.

Statement
---------

.. c:type:: drizzle_stmt_st

   The internal struct containing the prepared statment object

.. c:type:: drizzle_datetime_st

   The struct for passing a date/time to/from the prepared statement API

   .. c:member:: uint16_t year

      The year part of the date

   .. c:member:: uint8_t month

      The month part of the date

   .. c:member:: uint32_t day

      The day part of the date

   .. c:member:: uint16_t hour

      The hour part of the time

   .. c:member:: uint8_t minute

      The minute part of the time

   .. c:member:: uint8_t second

      The second part of the time

   .. c:member:: uint32_t microsecond

      The microsecond part of the time

   .. c:member:: bool negative

      Is the time negative


Binlog
------

.. c:type:: drizzle_binlog_st

   The internal struct containing a binlog event

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

   The internal drizzle connection object struct
