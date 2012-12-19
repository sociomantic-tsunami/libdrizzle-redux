.. program:: drizzle_binlog

Drizzle Binlog
==============

Introduction
------------

Drizzle binlog is an example tool which will connect to a MySQL server as a
slave, retrieve the binary log data and store it as local copies of the binary
log files.  It has a continuous mode which will continue to download new events
as they are applied.

Usage
-----

:program:`drizzle_binlog` :option:`--start-file=binlog_file <--start-file>` OPTIONS

Options
-------

Main Options
^^^^^^^^^^^^

.. option:: --host <hostname>

   The hostname for the MySQL server (default 'localhost')

.. option:: --port <port>

   The port number for the MySQL server (default 3306)

.. option:: --user <user>

   The MySQL user to connect as (default is the system user drizzle_binlog is executed as)

.. option:: --pass <pass>

   The MySQL password to connect using

.. option:: --outdir <outdir>

   The output directory to store the binlogs

Binlog Options
^^^^^^^^^^^^^^

.. option:: --start-file <file>

   Which binlog file should be requested first

.. option:: --start-pos <pos>

   Position in the binlog file to start at

.. option:: --continuous

   Connect as a random slave ID of 32768-65535 so drizzle_binlogs isn't
   disconnected at the end of the last log
