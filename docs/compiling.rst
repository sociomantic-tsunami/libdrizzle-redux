Compiling Libdrizzle Redux
==========================

Building Libdrizzle Redux
-------------------------

To build libdrizzle-redux you first need to invoke CMake to create the build
scripts::

   cmake .

If you want to change where it is installed you need to supply the install
prefix::

   cmake . -DCMAKE_INSTALL_PREFIX=/path/to/install

To make a static library you need to turn off shared library support::

   cmake . -DBUILD_SHARED_LIBS=OFF

You can of course combine multiples of these, CMake will give you a summary of
options once it has run.

Once CMake has run it is a simple case of::

   make
   make install

Linking Your Application
------------------------

To link your app to libdrizzle-redux you need to provide the following to GCC,
this assumes that the library is in your library and include paths::

   gcc app.c -oapp -ldrizzle-redux -lssl

