Buffered Results
================

Introduction
------------

In this example :c:func:`drizzle_query` is used to send a select query to a
MySQL server.  The whole result set is then retrieved and stored in memory
using :c:func:`drizzle_result_buffer`.

The number of columns is retrieved using :c:func:`drizzle_result_column_count`.
Each row is iterated through by calling :c:func:`drizzle_row_next` which
returns an array containing string of the row data.  We know how many elements
are in this array due to the earlier call to
:c:func:`drizzle_result_column_count`.  The data from each element in the row
is finally echoed to the console.

To end the query the result set is freed using :c:func:`drizzle_result_free`

Code
----

.. code-block:: c

    #include <libdrizzle-5.1/libdrizzle.h>
    #include <stdio.h>
    #include <stdlib.h>

    int main(int argc, char *argv[])
    {
      (void) argc;
      (void) argv;
      drizzle_st *con;
      drizzle_return_t ret;
      drizzle_result_st *result;
      drizzle_row_t row;
      int num_fields;

      con = drizzle_create("localhost", 3306, "user", "pass", "test", 0);
      if (con == NULL)
      {
        printf("Drizzle connection object creation error\n");
        return EXIT_FAILURE;
      }
      ret = drizzle_connect(con);
      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Drizzle connection failure\n");
        return EXIT_FAILURE;
      }

      result= drizzle_query(con, "select * from libdrizzle.t1", 0, &ret);
      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Select failure\n");
        return EXIT_FAILURE;
      }
      drizzle_result_buffer(result);
      num_fields= drizzle_result_column_count(result);

      printf("%d fields\n", num_fields);
      while ((row = drizzle_row_next(result)))
      {
        printf("Data: ");
        for (uint16_t col=0; col < num_fields; col++)
        {
          printf("%s", row[col]);
        }
        printf("\n");
      }

      drizzle_result_free(result);

      drizzle_quit(con);
      return EXIT_SUCCESS;
    }

