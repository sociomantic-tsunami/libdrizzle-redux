Prepared Statements
===================

Introduction
------------

This section outlines the prepared statement functionality

Functions
---------

.. c:function:: drizzle_stmt_st* drizzle_stmt_prepare(drizzle_st *con, const char *statement, size_t size, drizzle_return_t *ret_ptr)

   Prepare a new statememt

   :param con: A connection object
   :param statement: The prepared statement with question marks ('?') for the elements to be provided as parameters
   :param size: The length of the statement
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A newly allocated and prepared statement object (or NULL on error)

.. c:function:: drizzle_return_t drizzle_stmt_bind_param(drizzle_stmt_st *stmt, uint16_t param_num, drizzle_column_type_t type, void *data, uint32_t length, drizzle_bind_options_t options)

   Bind a parameter to send in statement.  This needs to be called before every
   :c:func:`drizzle_stmt_execute` if the data changes.

   :param stmt: The prepared statement object
   :param param_num: The parameter number this bind is for
   :param type: The column type this parameter uses
   :param data: The data for this parameter
   :param length: The length of the data
   :param options: Options for the parameter
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_execute(drizzle_stmt_st *stmt)

   Executes a prepared statement

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_send_long_data(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t *data, size_t len)

   Send long binary data packet

   :param stmt: The prepared statement object
   :param param_num: The parameter number this data is for
   :param data: A pointer to the data
   :param len: The length of the data
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_reset(drizzle_stmt_st *stmt)

   Reset a statement to the prepared state

   :param stmt: The prepared statment object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_fetch(drizzle_stmt_st *stmt)

   Fetch a row from the result set, can be used with buffered or unbuffered
   result sets

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_buffer(drizzle_stmt_st *stmt)

   Buffer the entire result set

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_close(drizzle_stmt_st *stmt)

   Close and free a prepared statement

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_column_type_t drizzle_stmt_item_type(drizzle_stmt_st *stmt, uint16_t column_number)

   Gets the column type of an element from a row retrieved with :c:func:`drizzle_stmt_fetch`

   :param stmt: The prepared statement object
   :param column_number: The column number for the element
   :returns: The column type

.. c:function:: void* drizzle_stmt_item_data(drizzle_stmt_st *stmt, uint16_t column_number)

   Gets the data of an element from a row retrieved with :c:func:`drizzle_stmt_fetch`

   :param stmt: The prepared statement object
   :param column_number: The column number for the element
   :returns: The data for the element

.. c:function:: uint32_t drizzle_stmt_item_length(drizzle_stmt_st *stmt, uint16_t column_number)

   Gets the length of the data for an element from a row retrieved with :c:func:`drizzle_stmt_fetch`

   :param stmt: The prepared statement object
   :param column_number: The column number for the element
   :returns: The length of data for the element

.. c:function:: drizzle_bind_options_t drizzle_stmt_item_options(drizzle_stmt_st *stmt, uint16_t column_number)

   Gets the options set for an element from a row retrieved with :c:func:`drizzle_stmt_fetch`

   :param stmt: The prepared statement object
   :param column_number: The column number for the element
   :returns: The options for the element

.. c:function:: uint16_t drizzle_stmt_column_count(drizzle_stmt_st *stmt)

   Gets the column count for a result set which has been executed using :c:func:`drizzle_stmt_execute`

   :param stmt: The prepared statement object
   :returns: The column count

.. c:function:: uint64_t drizzle_stmt_affected_rows(drizzle_stmt_st *stmt)

   Gets the affected rows count for a result set which has been executed using :c:func:`drizzle_stmt_execute`

   :param stmt: The prepared statement object
   :returns: The column count

.. c:function:: uint64_t drizzle_stmt_insert_id(drizzle_stmt_st *stmt)

   Gets the insert ID for a result set which has been executed using :c:func:`drizzle_stmt_execute`

   :param stmt: The prepared statement object
   :returns: The insert ID

.. c:function:: uint16_t drizzle_stmt_param_count(drizzle_stmt_st *stmt)

   Gets the number of parameters expected for a result set that has been prepared with :c:func:`drizzle_stmt_prepare`

   :param stmt: The prepared statement object
   :returns: The number of parameters

.. c:function:: uint64_t drizzle_stmt_row_count(drizzle_stmt_st *stmt)

   Gets the row count for a statement buffered with :c:func:`drizzle_stmt_buffer`

   :param stmt: The prepared statement object
   :returns: The row count
