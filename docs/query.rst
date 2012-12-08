Query Functions
===============

Introduction
------------

This section outlines the query and result functions

Functions
---------

.. c:function:: drizzle_result_st* drizzle_query(drizzle_con_st *con, const char *query, size_t size, drizzle_return_t *ret_ptr)

   Executes a query and returns a newly allocated result struct

   :param con: A connection object
   :param query: The query to execute
   :param size: The length of the query string
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: drizzle_result_st* drizzle_query_str(drizzle_con_st *con, const char *query, drizzle_return_t *ret_ptr)

   Executes a query using :c:func:`strlen` to calculate the length of the query
   string and returns a newly allocated result struct

   :param con: A connection object
   :param query: The query to execute
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: ssize_t drizzle_escape_string(char *to, const size_t max_to_size, const char *from, const size_t from_size)

   Escape a string for an SQL query

   :param to: the destination string
   :param max_to_size: the maximum length of the 'to' parameter
   :param from: the source string
   :param from_size: the length of the source string
   :returns: the length of the 'to' string or -1 upon error due to empty parameters or overflow

.. c:function:: bool drizzle_hex_string(char *to, const uint8_t *from, const size_t from_size)

   Convert data into a hexadecimal string of the data.

   .. note::
      The allocated length of the 'to' string should always be at least double
      the length of the 'from' string

   :param to: the destination string
   :param from: the source string
   :param from_size: the length of the source string
   :returns: a true upon success or false upon empty parameters

.. c:function:: bool drizzle_mysql_password_hash(char *to, const char *from, const size_t from_size)

   Convert data into a MySQL password hash string.

   .. note::
      The allocated length of the 'to' string should always be at least 41 bytes

   :param to: the destination string
   :param from: the source string:
   :param from_size: the length of the source string
   :returns: a true upons success or false upon empty parameters
