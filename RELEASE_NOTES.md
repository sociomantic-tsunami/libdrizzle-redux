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

New Features
============

* `drizzle_escape_str`

    This new function matches the signature of `drizzle_escape_string`, except it takes an extra parameter
    to determine whether to escape pattern-matching characters ('%' and '_') or not.
    Calling `drizzle_escape_string` is equivalent to calling `drizzle_escape_str` with `false` as last parameter.


Bugs fixed
==========

- `drizzle_escape_string`

    This function now correctly escapes `\b` and `\t`
