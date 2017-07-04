Dependencies
============

Dependency | Version
-----------|---------
gcc        | >=4.8.x
autoconf   | >=2.61
zlib1g-dev | >=0.13.x
libtool    | >=2.x
openssl<sup>1</sup> | >=v1.x

<sup>1</sup> openssl is needed if libdrizzle-redux is compiled with support for
SSL connections.

As mentioned in the release notes for [v5.5.3](https://github.com/sociomantic-tsunami/libdrizzle-redux/releases/tag/v5.5.3)_ the MinGW build is broken.

Given the limited resources of the contributors we have decided not to fix this
and instead stop active support for building for Windows (cross-compile) using
MinGW.

However, at the moment there is no plans to remove the existing support for
MinGW from the codebase.
Thus, should any contributors wish to add MinGW support again, they are more
than welcome to do so.

New Features
============

Issues fixed
============
