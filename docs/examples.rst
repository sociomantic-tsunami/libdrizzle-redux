Examples
========

Buffered Results
----------------

.. code-block:: c

    #include <libdrizzle/drizzle_client.h>
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


Unbuffered Results
------------------

.. code-block:: c

    #include <libdrizzle/drizzle_client.h>
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
      con = drizzle_con_add_tcp(drizzle, "localhost", 3306, "root", "", "libdrizzle", 0);
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

      if (drizzle_column_buffer(result) != DRIZZLE_RETURN_OK)
      {
        printf("Column buffer failure\n");
        return EXIT_FAILURE;
      }
      num_fields= drizzle_result_column_count(result);

      printf("%d fields\n", num_fields);
      while(1)
      {
        row= drizzle_row_buffer(result, &ret);
        if (ret != DRIZZLE_RETURN_OK)
        {
          printf("Row retrieval error\n");
          break;
        }
        if (row == NULL)
        {
          // EOF
          break;
        }
        printf("Data: ");
        for (uint16_t col=0; col < num_fields; col++)
        {
          printf("%s", row[col]);
        }
        printf("\n");
        drizzle_row_free(result, row);
      }

      drizzle_result_free(result);


      drizzle_con_quit(con);
      drizzle_free(drizzle);
      return EXIT_SUCCESS;
    }


Binlog Retrieval
----------------

.. code-block:: c

    #include <libdrizzle/drizzle_client.h>
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
      drizzle_binlog_st *binlog_event;

      drizzle = drizzle_create();
      if (drizzle == NULL)
      {
        printf("Drizzle object creation error\n");
        return EXIT_FAILURE;
      }
      con = drizzle_con_add_tcp(drizzle, "localhost", 3306, "root", "", "", 0);
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

      result= drizzle_start_binlog(con, 0, "", 0, &ret);
      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Drizzle binlog start failure\n");
        return EXIT_FAILURE;
      }

      while (ret == DRIZZLE_RETURN_OK)
      {
        uint32_t i;
        binlog_event= drizzle_binlog_get_event(result, &ret);
        if (ret != DRIZZLE_RETURN_OK)
          break;
        printf("Timestamp: %"PRIu32"\n", binlog_event->timestamp);
        printf("Type: %"PRIu8"\n", binlog_event->type);
        printf("Server-id: %"PRIu32"\n", binlog_event->server_id);
        printf("Next-pos: %"PRIu32"\n", binlog_event->next_pos);
        printf("Length: %"PRIu32"\n", binlog_event->length);
        printf("Data: 0x");
        for (i=0; i<binlog_event->length; i++)
          printf("%02X ", binlog_event->data[i]);
        printf("\n\n");
      }

      drizzle_con_quit(con);
      drizzle_free(drizzle);
      return EXIT_SUCCESS;
    }

