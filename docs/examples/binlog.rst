Binlog Retrieval
================

.. code-block:: c

    #include <libdrizzle-5.1/libdrizzle.h>
    #include <stdio.h>
    #include <stdlib.h>

    void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
    {
      (void) context;
      if (ret != DRIZZLE_RETURN_EOF)
      {
        printf("Error retrieving binlog: %s\n", drizzle_error(connection));
      }
    }


    void binlog_event(drizzle_binlog_event_st *event, void *context)
    {
      (void) context;
      printf("Timestamp: %"PRIu32"\n", drizzle_binlog_event_timestamp(event));
      printf("Type: %"PRIu8"\n", drizzle_binlog_event_type(event));
      printf("Server-id: %"PRIu32"\n", drizzle_binlog_event_server_id(event));
      printf("Next-pos: %"PRIu32"\n", drizzle_binlog_event_next_pos(event));
      length= drizzle_binlog_event_length(event);
      printf("Length: %"PRIu32"\n", length);
      data= drizzle_binlog_event_data(event);
      printf("Data: 0x");
      for (i=0; i<length; i++)
        printf("%02X ", data[i]);
      printf("\n\n");
    }



    int main(int argc, char *argv[])
    {
      (void) argc;
      (void) argv;
      drizzle_st *con;
      drizzle_return_t ret;
      drizzle_binlog_st *binlog;

      con = drizzle_create("localhost", 3306, "root", "", "", 0);
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

      binlog= drizzle_binlog_init(con, binlog_event, binlog_error, NULL, true);
      ret= drizzle_binlog_start(binlog, 0, "", 0);
      if (ret != DRIZZLE_RETURN_EOF)
      {
        printf("Drizzle binlog start failure\n");
        return EXIT_FAILURE;
      }



      drizzle_quit(con);
      return EXIT_SUCCESS;
    }

