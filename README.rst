Description |travis| |codecov_minor| |codecov_major|
====================================================

**Drizzle Redux** is a project which aims to breathe new life into the **libdrizzle**
C connector. It is designed to allow you to syn- or asynchronously connect to and query a
**MySQL** database server using a simple API.

**Libdrizzle-redux** is a fork of the official **v5.1.4 Felmingham** release available
at https://launchpad.net/libdrizzle.

The primary reason for the fork is that development of the upstream stopped in 2013
which meant that crucial fixes and enhancements were not included in the latest
stable release **v5.1.4 Felmingham**.

The repository is part of the `Tsunami programme`_ created by `sociomantic labs GmbH`_
in order to support developers who want to contribute to the open source community.

We hope that continuing the development of **libdrizzle-redux** can benefit current
and future users of the library.

For more details on new features please refer to the
`release notes`_

Support Guarantees
------------------

- Major branch development period: 6 months
- Maintained minor versions: 1 most recent

Maintained Major Branches
-------------------------

.. csv-table::
   :header: "Major", "Initial release date", "Supported until"
   :widths: 5, 15, 12

   `v6.x.x`_, `v6.0.0`_: 2017-04-06, TBA - 6 months after next major release

.. _v6.x.x: https://github.com/sociomantic-tsunami/libdrizzle-redux/tree/v6.x.x
.. _v6.0.0: https://github.com/sociomantic-tsunami/libdrizzle-redux/tree/v6.0.0

Releases
========

`Latest stable release notes`_ | `All`_

The release process **libdrizzle-redux** is based on SemVer_ versioning as
described `here`_.
This means that the **major version** is increased for breaking changes, the **minor
version** is increased for feature releases, and the **patch version** is increased
for bug fixes that don't cause breaking changes.

Releases are handled using **GitHub** releases. The notes associated with a
major or minor **GitHub** release are designed to help developers to migrate from
one version to another. The changes listed are the steps you need to take to
move from the previous version to the one listed.

Installing Libdrizzle Redux
===========================

There are several options for installing the library:

- The project's **apt** repository at `bintray`_ offers ``deb`` packages for Ubuntu Xenial.
- Build from source.
- Build and install the ``deb`` or ``rpm`` packages locally.

Please refer to the section `Installing Libdrizzle Redux`_ in the documentation for more info.

.. _`bintray`: https://bintray.com/sociomantic-tsunami/libdrizzle-redux/libdrizzle-redux
.. _Installing Libdrizzle Redux: ./docs/compiling.rst#installing-libdrizzle-redux


Building libdrizzle-redux
=========================

**Supported compilers**

.. csv-table::
  :header: "Compiler","Version"

   GNU gcc, >=4.5.x
   LLVM clang, >=3.3
   Apple LLVM clang [#]_ , >=6.1

.. [#] The version listed for Apple LLVM is the compiler used in the OS X builds
       on Travis CI. However earlier versions should be compatible as long as
       they support C++11 features, i.e. Apple LLVM 5.0, Xcode 5.0 and later.


**Other Dependencies**

.. csv-table::
   :header: "Dependency", "Version"

   autoconf, >=2.61
   zlib1g-dev, >=0.13.x
   libtool, >=2.x
   libssl-dev [#]_, >=v1.x

.. [#] openssl is needed if libdrizzle-redux is compiled with support for
       SSL connections.

To build **libdrizzle-redux** you can invoke bootstrap script::

    bootstrap.sh

Alternatively you can build and customize::

    mkdir build && cd build
    autoreconf -fi ..
    ../configure
    make
    make install

For more information about compiling **libdrizzle-redux** please
refer to `compiling.rst`_ or build the documentation, cf. below.

**Linking libdrizzle-redux**::

Ensure the library is in your library and include paths. For releases prior to
version ``v6.0.2`` [2]_ linking your app against libdrizzle-redux requires the flag
``-ldrizzle-redux``::

    g++ app.c -oapp -ldrizzle-redux -lssl -lcrypto -pthread

From version ``v6.0.3`` and later the API level of the library is appended to
the installed library name. This is also reflected in the install path for
development headers which now follows the pattern::

    /<include-prefix>/libdrizzle-redux[MAJOR_VERSION]/libdrizzle-redux

Thus, linking against ``libdrizzle-redux v6.0.3`` requires the flag
``-ldrizzle-redux6`` and if headers are included to add
``-I/<prefix>/libdrizzle-redux6``, e.g::

    g++ app.c -oapp -I/usr/include/libdrizzle-redux6 -ldrizzle-redux6 -lssl -lcrypto -pthread

Another option is to link against libdrizzle-redux using the full name of the
dynamic library, e.g.::

    g++ app.c -oapp -I/usr/include/libdrizzle-redux6 -l:libdrizzle-redux6.so.13 -lssl -lcrypto -pthread

.. [2] v6.0.2 added the major version to the package name and the library file
       but the release is deprecated since the linking did not work correctly.

**Building documentation**

Documentation can be generated in several formats::

    pdf  : make latexpdf
    epub : make epub
    html : make html

Contributing to libdrizzle-redux
--------------------------------

We appreciate any contributions to the development of **libdrizzle-redux**.
One requirement is that the changes should be added in accordance with a
versioning scheme based on SemVer.
A set of guidelines guidelines and tools to help developers and users are
available at the wiki of the `Neptune`_ project, which is also part of
the `Tsunami programme`_ created by `sociomantic labs GmbH`_.

.. |travis| image:: https://travis-ci.org/sociomantic-tsunami/libdrizzle-redux.svg?branch=master
   :alt: Travis Build Status
   :target: https://travis-ci.org/sociomantic-tsunami/libdrizzle-redux
.. _SemVer: http://semver.org
.. _Latest stable release notes: https://github.com/sociomantic-tsunami/libdrizzle-redux/releases/latest
.. _release notes: https://github.com/sociomantic-tsunami/libdrizzle-redux/releases/latest
.. _All: https://github.com/sociomantic-tsunami/libdrizzle-redux/releases/
.. _here: https://github.com/sociomantic-tsunami/neptune/blob/master/doc/library-user.rst
.. _compiling.rst: https://github.com/andreas-bok-sociomantic/libdrizzle-redux/blob/v5.4.x/docs/compiling.rst
.. _Tsunami programme: https://github.com/sociomantic-tsunami
.. _sociomantic labs GmbH: https://www.sociomantic.com
.. _Neptune: https://github.com/sociomantic-tsunami/neptune/blob/master/doc/library-user.rst#contributing-to-a-neptune-versioned-library
.. |codecov_major| image:: https://codecov.io/gh/bokchan/libdrizzle-redux/branch/v6.x.x/graph/badge.svg
   :alt: Coveralls Build Status v6.x.x
   :target: https://codecov.io/gh/bokchan/libdrizzle-redux
.. |codecov_minor| image:: https://codecov.io/gh/bokchan/libdrizzle-redux/branch/v6.2.x/graph/badge.svg
  :alt: Coveralls Build Status v6.2.x
  :target: https://codecov.io/gh/bokchan/libdrizzle-redux
