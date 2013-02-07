Query Functions
===============

Introduction
------------

This section outlines the query and result functions

Structs
-------

.. c:type:: drizzle_query_st

   The internal query object struct

.. c:type:: drizzle_result_st

   The internal result object struct

.. c:type:: drizzle_column_st

   The internal column object struct

Functions
---------

.. c:function:: drizzle_return_t drizzle_set_ssl(drizzle_st *con, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher)

   Sets the SSL data

   :param con: A connection object
   :param key: The path to a key file
   :param cert: The path to a certificate file
   :param ca: The path to a certificate authority file
   :param capath: The path to a directory that contains trusted CA certificate files
   :param cipher: A list of allowed ciphers for SSL encryption
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_result_st* drizzle_query(drizzle_st *con, const char *query, size_t size, drizzle_return_t *ret_ptr)

   Executes a query and returns a newly allocated result struct

   :param con: A connection object
   :param query: The query to execute
   :param size: The length of the query string, if set to 0 then :c:func:`strlen` is used to calculate the length
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated result object

.. c:function:: ssize_t drizzle_escape_string(drizzle_st *con, char **to, const const char *from, const size_t from_size)

   Escape a string for an SQL query.  The ``to`` parameter is allocated by the function and needs to be freed by the application when finished with.

   :param con: a connection object
   :param to: the destination string
   :param from: the source string
   :param from_size: the length of the source string
   :returns: the length of the 'to' string or -1 upon error due to empty parameters or overflow

.. c:function:: void drizzle_result_free(drizzle_result_st *result)

   Frees a result object

   :param result: the result set to free

.. c:function:: void drizzle_result_free_all(drizzle_st *con)

   Frees all result objects for a given connection object

   :param con: A connection object

.. c:function:: drizzle_st* drizzle_result_drizzle_con(drizzle_result_st *result)

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

.. c:function:: drizzle_result_st* drizzle_result_read(drizzle_st *con, drizzle_return_t *ret_ptr)

   Reads the next result in a multi-result return

   :param con: A connection object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The result struct for the new object

.. c:function:: drizzle_return_t drizzle_result_buffer(drizzle_result_st *result)

   Buffers a result set

   :param result: A result object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_result_st* drizzle_column_drizzle_result(drizzle_column_st *column)

   Gets a result set for a given column object

   :param column: A column object
   :returns: A result object

.. c:function:: const char* drizzle_column_catalog(drizzle_column_st *column)

   Gets the catalog name for a given column

   :param column: A column object
   :returns: The catalog name

.. c:function:: const char* drizzle_column_db(drizzle_column_st *column)

   Gets the database name for a given column

   :param column: A column object
   :returns: The database name

.. c:function:: const char* drizzle_column_table(drizzle_column_st *column)

   Get the table name (or table alias) for a given column

   :param column: A column object
   :returns: The table name

.. c:function:: const char* drizzle_column_orig_table(drizzle_column_st *column)

   Gets the original table name (if an alias has been used) for a given column

   :param column: A column object
   :returns: The original table name

.. c:function:: const char* drizzle_column_name(drizzle_column_st *column)

   Gets the column name (or column alias) for a given column

   :param column: A column object
   :returns: The column name

.. c:function:: const char* drizzle_column_orig_name(drizzle_column_st *column)

   Gets the original column name (if an alias has been used) for a given column

   :param column: A column object
   :returns: The original column name

.. c:function:: drizzle_charset_t drizzle_column_charset(drizzle_column_st *column)

   Gets the character set ID for a given column

   :param column: A column object
   :returns: The character set ID

.. c:function:: uint32_t drizzle_column_size(drizzle_column_st *column)

   Gets the size of a given column

   :param column: A column object
   :returns: The column size

.. c:function:: size_t drizzle_column_max_size(drizzle_column_st *column)

   Gets the maximum size of a given column

   :param column: A column object
   :returns: The maximum size

.. c:function:: drizzle_column_type_t drizzle_column_type(drizzle_column_st *column)

   Gets the type of data for the column

   :param column: A column object
   :returns: The column type

.. c:function:: drizzle_column_flags_t drizzle_column_flags(drizzle_column_st *column)

   Gets the flags for a given column

   :param column: A column object
   :returns: The column flags

.. c:function:: uint8_t drizzle_column_decimals(drizzle_column_st *column)

   Gets the number of decimal places for a given column

   :param column: A column object
   :returns: The number of decimal places

.. c:function:: const unsigned char* drizzle_column_default_value(drizzle_column_st *column, size_t *size)

   Gets the default value for a given column

   :param column: A column object
   :returns: A string containing the default value

.. c:function:: drizzle_return_t drizzle_column_skip(drizzle_result_st *result)

   Skips the next column in a result set when using
   :c:func:`drizzle_column_read` to get the column data

   :param result: A result object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: void drizzle_column_free(drizzle_column_st *column)

   Frees a column when using :c:func:`drizzle_column_read` to get the column
   data

   :param column: The column to be freed

.. c:function:: drizzle_column_st* drizzle_column_read(drizzle_result_st *result, drizzle_return_t *ret_ptr)

   Reads a column from network buffer

   :param result: A result object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated column

.. c:function:: drizzle_return_t drizzle_column_buffer(drizzle_result_st *result)

   Buffers all the columns for a result set

   :param result: A result object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_column_st* drizzle_column_next(drizzle_result_st *result)

   Gets the next column in a buffered column result set

   :param result: A result object
   :returns: A column object

.. c:function:: drizzle_column_st* drizzle_column_prev(drizzle_result_st *result)

   Gets the previous column in a buffered column result set

   :param result: A result object
   :returns: A column object

.. c:function:: void drizzle_column_seek(drizzle_result_st *result, uint16_t column)

   Seeks to a given column in a buffered column result set

   :param result: A result object
   :param column: The column number

.. c:function:: drizzle_column_st* drizzle_column_index(drizzle_result_st *result, uint16_t column)

   Gets a given column in a column buffered result set

   :param result: A result object
   :param column: The column number
   :returns: A column object

.. c:function:: uint16_t drizzle_column_current(drizzle_result_st *result)

   Gets the column number in a buffered or unbuffered column result set

   :param result: A result object:
   :returns: The column number

.. c:function:: uint64_t drizzle_row_read(drizzle_result_st *result, drizzle_return_t *ret_ptr)

   Reads the next row header and returns the row number for unbuffered row
   reads.  Use :c:func:`drizzle_field_read` or :c:func:`drizzle_field_buffer`
   to get the field data after this call.

   :param result: A result object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The row number

.. c:function:: drizzle_row_t drizzle_row_buffer(drizzle_result_st *result, drizzle_return_t *ret_ptr)

   Read and buffer one entire row, must be freed with c:func:`drizzle_row_free`

   :param result: A result object
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The newly allocated row buffer

.. c:function:: void drizzle_row_free(drizzle_result_st *result, drizzle_row_t row)

   Free a buffered row read

   :param result: A result object
   :param row: The row data to be freed

.. c:function:: size_t* drizzle_row_field_sizes(drizzle_result_st *result)

   Gets an array of the field sizes for buffered rows

   :param result: A result object
   :returns: An array of row sizes

.. c:function:: drizzle_row_t drizzle_row_next(drizzle_result_st *result)

   Gets the next row in a buffered result set

   :param result: A result object
   :returns: The row data

.. c:function:: drizzle_row_t drizzle_row_prev(drizzle_result_st *result)

   Gets the previous row in a buffered result set

   :param result: A result object
   :returns: The row data

.. c:function:: void drizzle_row_seek(drizzle_result_st *result, uint64_t row)

   Seeks to a given row in a buffered result set

   :param result: A result object
   :param row: The row number to seek to

.. c:function:: drizzle_row_t drizzle_row_index(drizzle_result_st *result, uint64_t row)

   Gets a row at the given index in a buffered result set

   :param result: A result object
   :param row: The row number to get
   :returns: The row data

.. c:function:: uint64_t drizzle_row_current(drizzle_result_st *result)

   Gets the current row number

   :param result:  A result object
   :returns: The row number

.. c:function:: drizzle_field_t drizzle_field_read(drizzle_result_st *result, size_t *offset, size_t *size, size_t *total, drizzle_return_t *ret_ptr)

   Reads the next field from the network buffer.  Useful for large blobs
   without buffering the entire blob.

   :param result: A result object
   :param offset: The offset position of the blob for this read, to be written to by the function
   :param size: The size of the read, to be written to by the function
   :param total: The total size of the field, to be written to by the function
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The field data

.. c:function:: drizzle_field_t drizzle_field_buffer(drizzle_result_st *result, size_t *total, drizzle_return_t *ret_ptr)

   Read and buffer the entire field for an unbuffered row read.

   :param result: A result object
   :param total: The total size of the field, to be written to by the function
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: The field data

.. c:function:: void drizzle_field_free(drizzle_field_t field)

   Frees field data for unbuffered row reads

   :param field: The field data to free
