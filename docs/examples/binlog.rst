Binlog Retrieval
================

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

      con = drizzle_create_tcp("localhost", 3306, "root", "", "", 0);
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

      result= drizzle_start_binlog(con, 0, "", 0, &ret);
      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Drizzle binlog start failure\n");
        return EXIT_FAILURE;
      }

      while (ret == DRIZZLE_RETURN_OK)
      {
        uint32_t i, length;
        const unsigned char *data;
        ret= drizzle_binlog_get_event(result);
        if (ret != DRIZZLE_RETURN_OK)
          break;
        printf("Timestamp: %"PRIu32"\n", drizzle_binlog_event_timestamp(result));
        printf("Type: %"PRIu8"\n", drizzle_binlog_event_type(result));
        printf("Server-id: %"PRIu32"\n", drizzle_binlog_event_server_id(result));
        printf("Next-pos: %"PRIu32"\n", drizzle_binlog_event_next_pos(result));
        length= drizzle_binlog_event_length(result);
        printf("Length: %"PRIu32"\n", length);
        data= drizzle_binlog_event_data(result);
        printf("Data: 0x");
        for (i=0; i<length; i++)
          printf("%02X ", data[i]);
        printf("\n\n");
      }

      drizzle_quit(con);
      return EXIT_SUCCESS;
    }

