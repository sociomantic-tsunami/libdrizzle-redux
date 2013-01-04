Prepared Statements
===================

Introduction
------------

This code example has been simplified to make it easier to read, the connection
and error handling code has been removed.

In this example a basic select query has been defined which has one element to
replace using the '?' character.  The statement is prepared using
:c:func:`drizzle_stmt_prepare` and we can get the number of parameters the
server is expecting with :c:func:`drizzle_stmt_param_count`.  In this example
we know that there is only one parameter required so we send one INT type
parameter using :c:func:`drizzle_stmt_set_int` stating that this is
parameter 0 and a signed value.

Once the parameters have been provided the statement is executed using
:c:func:`drizzle_stmt_execute` and the results buffered using
:c:func:`drizzle_stmt_buffer`.  Once buffered the row count can be obtained
using :c:func:`drizzle_stmt_row_count`.

Finally we get the result data.  A call to :c:func:`drizzle_stmt_fetch` gets
the next row from either the network or the buffer (the buffer in this case).
The int data is retreived using :c:func:`drizzle_stmt_get_int`, a call for
each column in the row (in example the table only has one column) is made using
the ``drizzle_stmt_get_`` functions.

When we are done the statement is closed and cleaned up using
:c:func:`drizzle_stmt_close`.  It can also be reused with
:c:func:`drizzle_stmt_reset` or executed again with
:c:func:`drizzle_stmt_execute` (this is useful for inserts).

Code
----

.. code-block:: c

  drizzle_stmt_st *stmt;
  const char *query= "select * from libdrizzle.t1 where a > ?";
  stmt= drizzle_stmt_prepare(con, query, strlen(query), &ret);

  printf("Params: %" PRIu16 "\n", drizzle_stmt_param_count(stmt));

  uint32_t val= 1;
  ret = drizzle_stmt_set_int(stmt, 0, val, false);

  ret = drizzle_stmt_execute(stmt);

  ret = drizzle_stmt_buffer(stmt);

  printf("Rows found: %" PRIu64 "\n", drizzle_stmt_row_count(stmt));
  while (drizzle_stmt_fetch(stmt) != DRIZZLE_RETURN_ROW_END)
  {
    uint32_t res_val;
    res_val= drizzle_stmt_get_int(stmt, 0, &ret);
    printf("Got value: %" PRIu32 "\n", *res_val);
  }
  ret = drizzle_stmt_close(stmt);

