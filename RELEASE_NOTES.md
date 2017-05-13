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

Migration Notes
===============

- For users upgrading from `v5.x` the installation removes the outdated symlink from `/usr/lib/libdrizzle-redux.so.10` to `/usr/lib/libdrizzle-redux.so.9`.

New Features
============
