Description
===========

Drizzle Redux is a project which aims to breath new life into the libdrizzle
C connector. It is designed to allow you to connect to and query a
MySQL database server using a simple API.

Libdrizzle-redux in the sociomantic repository is a fork of the official
libdrizzle redux at https://launchpad.net/libdrizzle.

Releases
========

`Latest stable release notes
<https://github.com/sociomantic/libdrizzle-redux/releases/latest>`_ | `Current,
in development, release notes
<https://github.com/sociomantic/libdrizzle-redux/blob/master/RELEASE_NOTES.md>`_ | `All
releases <https://github.com/sociomantic/libdrizzle-redux/releases>`_

Releases are handled using `GitHub releases
<https://github.com/sociomantic/libdrizzle-redux/releases>`_. The release notes
provided there are usually structured in 3 sections, a **Migration Instructions**,
which are the mandatory steps the users have to do to update to a new version,
**Deprecated** which contains deprecated functions which is recommended not to
use but will not break any old code, and the **New Features** which are
optional new features available in the new version that users might find
interesting.  Using them is optional, but encouraged.

These instructions should help developers to migrate from one version to
another. The changes listed here are the steps you need to take to move from
the previous version to the one being listed. For example, all the steps
described in version **v1.5** are the steps required to move from **v1.4** to
**v1.5**.

If you need to jump several versions at once, you should read all the steps from
all the intermediate versions. For example, to jump from **v1.2** to **v1.5**,
you need to first follow the steps in version **v1.3**, then the steps in
version **v1.4** and finally the steps in version **v1.5**.

There are also sometimes *patch-level* releases, in that case there are no
breaking changes or new features, just bug fixes, and thus, only bug fixes are
listed in the release notes.

Building libdrizzle-redux
^^^^^^^^^^^^^^^^^^^^^^^^^

To build libdrizzle-redux you can invoke bootstrap script::

    bootstrap.sh

Alternatively you can build and customize::

    autoreconf -fi
    ./configure
    make
    make install

For more information about compiling libdrizzle redux please
refer to ``compiling.rst`` in the ``./docs`` folder or build the documentation,
cf. below.

**Linking libdrizzle-redux**

``gcc app.c -oapp -ldrizzle-redux -lssl``

If libdrizzle-redux is installed alongisde other versions of libdrizzle,
the linking should be done with the full name of the dynamic library.

``gcc app.c -oapp -l:libdrizzle-redux.so.9 -lssl``

**Building documentation**

Documentation can be generated in several formats::

   pdf  : make latexpdf
   epub : make epub
   html : make html
