Event Callback
==============

Introduction
------------

In this example :c:func:`drizzle_event_watch_fn()` is used to set a custom I/O event
watcher function for a drizzle structure.
It is used to integrate libdrizzle-redux with a custom event loop. The callback will be invoked
to register or deregister interest in events for a connection. When the
events are triggered, :c:func:`drizzle_set_revents()` should be called to
indicate which events are ready. The event loop should stop waiting for
these events, as libdrizzle-redux will call the callback again if it is still
interested. To resume processing, the libdrizzle-redux function that returned
:py:const:`DRIZZLE_RETURN_IO_WAIT` should be called again.

The custom callback must have a signature as shown:

.. code-block:: c

    drizzle_return_t (drizzle_event_watch_fn)(drizzle_con_st *con, short events, void *context);

Code
----

.. code-block:: c

    #include <libdrizzle-5.1/libdrizzle.h>
    #include <libdrizzle-5.1/constants.h>

    extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
    void *context);

    extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
      void *context)
    {
      // The context defined for the drizzle connection
      int* drizzle_cxt = (int*) drizzle_context(con);

      // The context defined for the callback function
      int* callback_cxt = (int*) context;

      printf("Called drizzle_event_callback\n");

      return DRIZZLE_RETURN_OK;
    }

    int main(int argc, char *argv[])
    {
      (void) argc;
      (void) argv;

      int cxt_a = 1;
      int cxt_b = 0;

      drizzle_st *con = drizzle_create("localhost", 3306, "root", "", "libdrizzle", 0);
      if (con == NULL)
      {
        printf("Drizzle connection object creation error\n");
        return EXIT_FAILURE;
      }

      // Set drizzle dummy context
      drizzle_set_context(con, (void*)&cxt_a);

      // Set user defined callback function event_watch_fn
      drizzle_set_event_watch_fn(con, drizzle_event_callback, (void*)&cxt_b);

      drizzle_return_t driz_ret= drizzle_connect(con);

      if (ret != DRIZZLE_RETURN_OK)
      {
        printf("Drizzle connection failure\n");
        return EXIT_FAILURE;
      }

      driz_ret= drizzle_quit(con);

      return EXIT_SUCCESS;
    }
