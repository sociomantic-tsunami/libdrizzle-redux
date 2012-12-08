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

.. c:function:: void drizzle_result_free(drizzle_result_st *result)

   Frees a result object

   :param result: the result set to free

.. c:function:: void drizzle_result_free_all(drizzle_con_st *con)

   Frees all result objects for a given connection object

   :param con: A connection object

.. c:function:: drizzle_con_st* drizzle_result_drizzle_con(drizzle_result_st *result)

   Gets the connection object from a given result object

   :param result: A result object
   :returns: The connection object associated to the result object

.. c:function:: bool drizzle_result_eof(drizzle_result_st *result)

   Tests to see if an EOF packet has been hit

   :param result: A result object
   :returns: true on EOF or false

.. c:function:: const char* drizzle_result_message(drizzle_result_st *result)

   Get error or information message from result set

   :param result: A result object
   :returns: The message to be returned

.. c:function:: uint16_t drizzle_result_error_code(drizzle_result_st *result)

   Gets the error code from a result set

   :param result: A result object
   :returns: The error code

.. c:function:: const char* drizzle_result_sqlstate(drizzle_result_st *result)

   Gets the SQL state from a result set

   :param result: A result object
   :returns: The SQL state string

.. c:function:: uint16_t drizzle_result_warning_count(drizzle_result_st *result)

   Gets the warning count from a result set

   :param result: A result object
   :retuns: The warning count

.. c:function:: uint64_t drizzle_result_insert_id(drizzle_result_st *result)

   Gets the insert ID for an auto_increment column in a result set

   .. note::
      With a MySQL server this returns the first ID with multiple inserts in a
      query.

   :param result: A result object
   :returns: The insert ID

.. c:function:: uint64_t drizzle_result_affected_rows(drizzle_result_st *result)

   Gets the affected row count from a result set

   :param result: A result object
   :returns: The affected row count

.. c:function:: uint16_t drizzle_result_column_count(drizzle_result_st *result)

   Gets the column count from a result set

   :param result: A result object
   :returns: The column count

.. c:function:: uint64_t drizzle_result_row_count(drizzle_result_st *result)

   Gets the row count from a result set buffered with
   :c:func:`drizzle_result_buffer`

   :param result: A result object
   :returns: The row count

.. c:function:: drizzle_result_st* drizzle_result_read(drizzle_con_st *con, drizzle_return_t *ret_ptr)

   Reads the next result in a multi-result return

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The result struct for the new object

.. c:function:: drizzle_return_t drizzle_result_buffer(drizzle_result_st *result)

   Buffers a result set

   :param result: A result object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success


