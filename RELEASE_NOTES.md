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

The library is now built in a separate toplevel directory `build`.
The modified build process is as follows:

    mkdir build && cd build
    autoreconf -fi ..
    ../configure
    make
    make install

New Features
============
