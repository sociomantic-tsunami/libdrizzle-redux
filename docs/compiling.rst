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

Building For OSX (clang and gcc)
--------------------------------

You can compile the source code with the ``clang`` compiler provided by
**Xcode Command Line Tools**.
Alternatively you can use Homebrew_ to install a specific ``gcc`` or ``clang``
compiler. Regardless of the choice of compiler, you will need to install **Xcode**
and the **Xcode Command Line Tools**.

Compatible compilers:

+-----------------------+----------+
| Compiler              | Version  |
+-----------------------+----------+
| GNU gcc               |  >= 4.5  |
+-----------------------+----------+
| LLVM clang            |  >= 3.3  |
+-----------------------+----------+
| Apple LLVM clang [#]_ |  >= 6.1  |
+-----------------------+----------+

.. [#] The version listed for Apple LLVM is the compiler used in the OS X builds
       on Travis CI. However earlier versions should be compatible as long as
       they support C++11 features, i.e. Apple LLVM 5.0, Xcode 5.0 and later.

#. Install the dependencies specified in the `RELEASE NOTES`_ of the latest minor release.

#. Ensure **OpenSSL** headers are linked by creating a symlink::

      ln -sf "$(brew --prefix openssl)/include/openssl" /usr/local/include/openssl

   or pass the OpenSSL directory to ``configure`` using ``--with-openssl``::

      ./configure --with-openssl=$(brew --prefix openssl)

#. Optionally set the C and C++ compiler before running ``configure``, e.g.::

      autoreconf -fi
      CC=gcc-4.9 CXX=g++-4.9 ./configure
      make


Linking Your Application
------------------------

To link your app to libdrizzle-redux you need to provide the following to GCC,
this assumes that the library is in your library and include paths::

   gcc app.c -oapp -ldrizzle-redux -lpthread

A tool called **libdrizzle-redux_config** is included to also assist with this.

.. _RELEASE NOTES: https://github.com/sociomantic-tsunami/libdrizzle-redux/releases
.. _Homebrew: http://brew.sh
