Binlog Retrieval
================

Introduction
------------

This example shows how the binlog api can be used to retrieve the events from a
mysql binlog.

First a :c:type:`drizzle_binlog_st` is created by calling :c:func:`drizzle_binlog_init`
with an already initialized :c:type:`drizzle_st` connection. Two callbacks are
specified; ``binlog_event``, which is called every time a binlog event is
retrieved and ``binlog_error`` which is called when an error occurs while parsing
binlog-data.
In addition it is possible to pass a user context to be used for the callback
functions. When ``NULL`` is passed, as in this example, the context is unspecified.
Calling :c:func:`drizzle_binlog_start` starts the parsing of the binlog. The second
argument is the id of the server to connect to. It corresponds to the ``server_id``
variable defined in the **MySQL** config file. When 0 is passed as ``server_id``
the connection is disconnected automatically at the end of the last logfile.

In the callback function ``binlog_event`` several properties of the binlog event
are retrieved from the :c:type:`drizzle_binlog_event_st` object.
E.g. the type of binlog event, cf. :c:type:`drizzle_binlog_event_types_t`, is
retrieved by calling :c:func:`drizzle_binlog_event_type`.

The binlog api offers retrieval of information which is common for all binlog
event types. To retrieve information specific to each type, please refer to the
`MySQL documentation`_

When all binlog events have been parsed, ``binlog_error`` is called with a return
status :py:const:`DRIZZLE_RETURN_EOF` indicating that end of the last binlog logfile has
been reached.

To compile the example, save the code and run:

``g++ binlog_example.cxx -ldrizzle-redux -lpthread -o binlog_example``


Code
----

.. code-block:: c

    #include <libdrizzle-5.1/libdrizzle.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <inttypes.h>

    /**
    * @file binlog_example.cxx
    */

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
      uint length= drizzle_binlog_event_length(event);
      printf("Length: %"PRIu32"\n", length);
      const unsigned char *data= drizzle_binlog_event_data(event);
      printf("Data: 0x");
      for (uint i=0; i<length; i++)
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

      // Should be changed to the specifics of the MySQL installation
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

.. _MySQL documentation: https://dev.mysql.com/doc/internals/en/binary-log.html
