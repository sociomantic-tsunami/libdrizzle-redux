Compiling Libdrizzle Redux
==========================

Building Libdrizzle Redux
-------------------------

To build libdrizzle-redux you can invoke bootstrap script::

   bootstrap.sh

Alternatively you can build and customize::

   autoreconf -fi
   ./configure
   make
   make install

Linking Your Application
------------------------

To link your app to libdrizzle-redux you need to provide the following to GCC,
this assumes that the library is in your library and include paths::

   gcc app.c -oapp -ldrizzle -lssl

A tool called :program:`libdrizzle_config` is included to also assist with this.
