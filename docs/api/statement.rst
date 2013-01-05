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

.. c:function:: drizzle_return_t drizzle_stmt_set_tiny(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t value, bool is_unsigned)

   Sets a parameter of a prepared statement to a tinyint value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :param is_unsigned: Set to true if the parameter is unsigned
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_short(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t value, bool is_unsigned)

   Sets a parameter of a prepared statement to a short int value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :param is_unsigned: Set to true if the parameter is unsigned
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_int(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t value, bool is_unsigned)

   Sets a parameter of a prepared statement to an int value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :param is_unsigned: Set to true if the parameter is unsigned
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_bigint(drizzle_stmt_st *stmt, uint16_t param_num, uint64_t value, bool is_unsigned)

   Sets a parameter of a prepared statement to a bigint value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :param is_unsigned: Set to true if the parameter is unsigned
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_double(drizzle_stmt_st *stmt, uint16_t param_num, double value)

   Sets a parameter of a prepared statement to a double value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_float(drizzle_stmt_st *stmt, uint16_t param_num, float value)

   Sets a parameter of a prepared statement to a float value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_string(drizzle_stmt_st *stmt, uint16_t param_num, char *value, size_t length)

   Sets a parameter of a prepared statement to a string value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param value: The value to set the parameter
   :param length: The length of the value data
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_null(drizzle_stmt_st *stmt, uint16_t param_num)

   Sets a parameter of a prepared statement to a NULL value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_time(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t days, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds, bool is_negative)

   Sets a parameter of a prepared statement to a time value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param days: The number of days for the time
   :param hours: The number of hours for the time
   :param minutes: The number of minutes for the time
   :param seconds: The number of seconds for the time
   :param microseconds: The number of microseconds for the time
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_set_timestamp(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds)

   Sets a parameter of a prepared statement to a datetime/timestamp value

   :param stmt: A prepared statement object
   :param param_num: The parameter number to set (starting at 0)
   :param year: The year number for the timestamp
   :param month: The month number for the timestamp
   :param day: The day number for the timestamp
   :param hours: The hour number for the timestamp
   :param minutes: The minute number for the timestamp
   :param seconds: The minute number for the timestamp
   :param microseconds: The minute number for the timestamp
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_execute(drizzle_stmt_st *stmt)

   Executes a prepared statement

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

.. c:function:: drizzle_return_t drizzle_stmt_send_long_data(drizzle_stmt_st *stmt, uint16_t param_num, unsigned char *data, size_t len)

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

.. c:function:: bool drizzle_stmt_get_is_null(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

   Check if a column for a fetched row is set to NULL

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: True if NULL

.. c:function:: bool drizzle_stmt_get_is_null_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

   Check if a column for a fetched row is set to NULL using a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into, :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: True if NULL

.. c:function:: bool drizzle_stmt_get_is_unsigned(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

   Check if a column for a fetched row is unsigned

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: True if unsigned

.. c:function:: bool drizzle_stmt_get_is_unsigned_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

   Check if a column for a fetched row is unsigned using a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into, :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: True if unsigned

.. c:function:: const char *drizzle_stmt_get_string(drizzle_stmt_st *stmt, uint16_t column_number, size_t *len, drizzle_return_t *ret_ptr)

   Get the string value for a column of a fetched row (int types are automatically converted)

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param len: A pointer to a :c:type:`size_t` to store the result length into
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into
   :returns: A pointer to the string value

.. c:function:: const char *drizzle_stmt_get_string_from_name(drizzle_stmt_st *stmt, const char *column_name, size_t *len, drizzle_return_t *ret_ptr)

   Get the string value for a column of a fetched row (int types are automatically converted) using a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param len: A pointer to a :c:type:`size_t` to store the result length into
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into, :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: A pointer to the string value

.. c:function:: uint32_t drizzle_stmt_get_int(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

   Get the int value for a column of a fetched row

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred
   :returns: The int value

.. c:function:: uint32_t drizzle_stmt_get_int_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

   Get the int value for a column of a fetched row using a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred,  :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: The int value

.. c:function:: uint64_t drizzle_stmt_get_bigint(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

   Get the bigint value for a column of a fetched row

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred
   :returns: The bigint value

.. c:function:: uint64_t drizzle_stmt_get_bigint_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

   Get the bigint value for a column of a fetched row using a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred,  :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: The bigint value

.. c:function:: double drizzle_stmt_get_double(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

   Get the double value for a column of a fetched row

   :param stmt: The prepared statement object
   :param column_number: The column number to get (starting at 0)
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred
   :returns: The double value

.. c:function:: double drizzle_stmt_get_double_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

   Get the double value for a column of a fetched row from a column name

   :param stmt: The prepared statement object
   :param column_name: The column name to get
   :param ret_ptr: A pointer to a :c:type:`drizzle_return_t` to store the return status into :py:const:`DRIZZLE_RETURN_TRUNCATED` if a truncation has occurred, :py:const:`DRIZZLE_RETURN_NOT_FOUND` if the column name cannot be found
   :returns: The double value

.. c:function:: drizzle_return_t drizzle_stmt_close(drizzle_stmt_st *stmt)

   Close and free a prepared statement

   :param stmt: The prepared statement object
   :returns: A return status code, :py:const:`DRIZZLE_RETURN_OK` upon success

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
