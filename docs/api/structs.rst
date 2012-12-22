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
