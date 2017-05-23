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

Please check the `RELEASE NOTES`_ for a list of dependencies specific to the
version of the library you are trying to compile.

.. _test-suite:

Running the Test Suite
----------------------

Libdrizzle has a unit test suite, it needs a running MySQL server which has a
user that can create databases, tables and can connect as a MySQL slave.

The test suite uses system environment variables to find the MySQL server:

* ``MYSQL_SERVER`` - The hostname of the MySQL server (default localhost)
* ``MYSQL_PORT`` - The port number of the MySQL server (default 3306)
* ``MYSQL_USER`` - The username for the MySQL connection (default empty)
* ``MYSQL_PASSWORD`` - The password for the MySQL username (default empty)
* ``MYSQL_SCHEMA`` - The default database for the MySQL connection (default empty)

The test suite can then be run using ``make check`` or ``make distcheck`` for
testing a source distribution rather than the straight git branch.

To test with valgrind you can run the following::

``TESTS_ENVIRONMENT="./libtool --mode=execute valgrind --error-exitcode=1 --leak-check=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE" make check``

Building For Windows (cross-compile) [BROKEN]
---------------------------------------------

.. attention::
   Building the library using MinGW is unfortunately broken at the moment.
   We apologize for any inconveniences and promise to look into the issue as soon
   as possible.

The bootstrap script can go into MinGW mode to cross compile for 32bit Windows
targets.  To do this you need to follow the following steps (this guide assumes
you are running 64bit Fedora but other Linux based operating systems should be
similar).

#. Install MinGW, you will need these packages::

      mingw32-gcc
      mingw32-gcc-c++
      mingw32-zlib

#. Tell bootstrap to build using MinGW::

      ./bootstrap.sh mingw

The test suite can be run in wine, to do this follow these steps:

#. Install wine, you will need to install the ``wine`` package

#. Setup wine to find the MinGW dlls as follows:

   #. Run wine as follows to create the required wine home directory::

         WINEARCH=win32 wine

   #. Open the ``~/.wine/system.reg`` file for editing
   #. Find the section called ``[System\\CurrentControlSet\\Control\\Session Manager\\Environment]``
   #. Under this find the ``PATH`` setting and add the path to MinGW's ``bin``
      directory using the ``Z:`` drive. For Fedora 18 64bit this makes the entry

      ``"PATH"=str(2):"C:\\windows\\system32;C:\\windows;C:\\windows\\system32\\wbem;Z:\\usr\\i686-w64-mingw32\\sys-root\\mingw\\bin"``

#. Run the test suite as follows (see `Running the Test Suite`_ for more details
   and needed environment variables when running the test suite)::

      WINEARCH=win32 TESTS_ENVIRONMENT=wine make check

Linking Your Application
------------------------

Ensure the library is in your library and include paths. For releases prior to
version ``v6.0.2`` linking your app against libdrizzle-redux requires the flag
``-ldrizzle-redux``::

    g++ app.c -oapp -ldrizzle-redux -lssl -lcrypto -pthread

From version ``v6.0.3`` and later the API level of the library is appended to
the installed library name [1]_. This is also reflected in the install path for
development headers which now follows the pattern::

    /<include-prefix>/libdrizzle-redux[MAJOR_VERSION]/libdrizzle-redux

Thus, linking against ``libdrizzle-redux v6.0.3`` requires the flag
``-ldrizzle-redux6`` and if headers are included to add
``-I/<prefix>/libdrizzle-redux6``, e.g.::

    g++ app.c -oapp -I/usr/include/libdrizzle-redux6 -ldrizzle-redux6 -lssl -lcrypto -pthread

Another option is to link against libdrizzle-redux using the full name of the
dynamic library, e.g.::

    g++ app.c -oapp -I/usr/include/libdrizzle-redux6 -l:libdrizzle-redux6.so.13 -lssl -lcrypto -pthread

A tool called **libdrizzle-redux_config** is included to also assist with this.

.. _RELEASE NOTES: https://github.com/sociomantic-tsunami/libdrizzle-redux/releases

.. rubric:: Footnotes

.. [1] v6.0.2 added the major version to the package name and the library file
       but the release is deprecated since the linking did not work correctly.
