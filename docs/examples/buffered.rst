Buffered Results
================

.. code-block:: c

    #include <libdrizzle/libdrizzle.h>
    #include <stdio.h>
    #include <stdlib.h>

    int main(int argc, char *argv[])
    {
      (void) argc;
      (void) argv;
      drizzle_st *drizzle;
      drizzle_con_st *con;
      drizzle_return_t ret;
      drizzle_result_st *result;
      drizzle_row_t row;
      int num_fields;

      drizzle = drizzle_create();
      if (drizzle == NULL)
      {
        printf("Drizzle object creation error\n");
        return EXIT_FAILURE;
      }
      con = drizzle_con_add_tcp(drizzle, "localhost", 3306, "user", "pass", "test", 0);
      if (con == NULL)
      {
        printf("Drizzle connection object creation error\n");
        return EXIT_FAILURE;
      }
      ret = drizzle_con_connect(con);
      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Drizzle connection failure\n");
        return EXIT_FAILURE;
      }

      result= drizzle_query_str(con, "select * from libdrizzle.t1", &ret);
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

      drizzle_con_quit(con);
      drizzle_free(drizzle);
      return EXIT_SUCCESS;
    }

