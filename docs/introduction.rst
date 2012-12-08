Introduction
============

Drizzle Redux is a project which aims to breath new life into the libdrizzle
C connector.  It is designed to allow you to connect to and query a MySQL
database server using a simple API.

The connector is 3-clause BSD licensed so it can statically and dynamically
link with almost any other open source or commercial software.

Differences from Libdrizzle
---------------------------
 
* The server-side functionality has been removed, it no longer acts as both
  a client and server API.
* The Drizzle prototype library functions have been removed.  It now only talks
  to MySQL compatible servers.
* API functions have been simplified.  In Libdrizzle there was a big confusion
  over whether the application or library should be doing the allocation and
  freeing of objects.  It is now less ambiguous.
* New binlog API added.  The library can now connect as a slave or mysqlbinlog
  client and retrieve binlog events.

There are many more new features to come.

Differences from libmysqlclient
-------------------------------

* API is slightly different
* Currently missing server-side prepared statement support and protocol
  compression

These missing features are to be added.
