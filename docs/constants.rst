Constants
=========

Introduction
============

Libdrizzle Redux contains a number of constants, most of what are in the form
of ENUMs.  All ENUMs are typedef'd so no need to use the 'enum' keyword.

Return
------

.. c:type:: drizzle_return_t

   Function return status ENUM

   .. py:data:: DRIZZLE_RETURN_OK
   
      Return is OK

   .. py:data:: DRIZZLE_RETURN_IO_WAIT

      Waiting on IO

   .. py:data:: DRIZZLE_RETURN_PAUSE

   .. py:data:: DRIZZLE_RETURN_ROW_BREAK

      Row break because row is larger than packet size

   .. py:data:: DRIZZLE_RETURN_MEMORY

      Memory allocation error

   .. py:data:: DRIZZLE_RETURN_ERRNO

      OS error code

   .. py:data:: DRIZZLE_RETURN_INTERNAL_ERROR

      Internal error during handshake

   .. py:data:: DRIZZLE_RETURN_GETADDRINFO

      Domain lookup failure

   .. py:data:: DRIZZLE_RETURN_NOT_READY

      Client is not connected to server

   .. py:data:: DRIZZLE_RETURN_BAD_PACKET_NUMBER

      Packets are out of sequence

   .. py:data:: DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET

      Bad packet received during handshake

   .. py:data:: DRIZZLE_RETURN_BAD_PACKET

      Bad packet received (unused)

   .. py:data:: DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED

      Attempt to connect to a version of MySQL less than 4.1

   .. py:data:: DRIZZLE_RETURN_UNEXPECTED_DATA

      Unexpected data in the receive buffer

   .. py:data:: DRIZZLE_RETURN_NO_SCRAMBLE

      No password scrable received (usually if server is expecting an auth
      plugin but client didn't use one)

   .. py:data:: DRIZZLE_RETURN_AUTH_FAILED

      Authentication failure

   .. py:data:: DRIZZLE_RETURN_NULL_SIZE

      Internal status

   .. py:data:: DRIZZLE_RETURN_ERROR_CODE

      Error code received from MySQL server

   .. py:data:: DRIZZLE_RETURN_TOO_MANY_COLUMNS

      Unused

   .. py:data:: DRIZZLE_RETURN_ROW_END

      Internal status

   .. py:data:: DRIZZLE_RETURN_LOST_CONNECTION

      Connection failure

   .. py:data:: DRIZZLE_RETURN_COULD_NOT_CONNECT

      Could not connect to server

   .. py:data:: DRIZZLE_RETURN_NO_ACTIVE_CONNECTIONS

      Waiting on a connection which doesn't exist (this shouldn't happen)

   .. py:data:: DRIZZLE_RETURN_HANDSHAKE_FAILED

      Handshake failure

   .. py:data:: DRIZZLE_RETURN_TIMEOUT

      Timeout during connection

   .. py:data:: DRIZZLE_RETURN_INVALID_ARGUMENT

      Bad arguments supplied to a function

   .. py:data:: DRIZZLE_RETURN_SSL_ERROR

      An error occured during SSL handshake

   .. py:data:: DRIZZLE_RETURN_EOF

      No more data to retrieve

Binlog
------

.. c:type:: DRIZZLE_BINLOG_MAGIC

   The 4-byte binlog header string

.. c:type:: drizzle_binlog_event_types_t

   An ENUM of binlog event types

   .. py:data:: DRIZZLE_EVENT_TYPE_UNKNOWN

      An unknown event

   .. py:data:: DRIZZLE_EVENT_TYPE_START

      A binlog start event

   .. py:data:: DRIZZLE_EVENT_TYPE_QUERY

      A MySQL query for SBR

   .. py:data:: DRIZZLE_EVENT_TYPE_STOP

      Binlog end event

   .. py:data:: DRIZZLE_EVENT_TYPE_ROTATE

      Binlog file rotate event

   .. py:data:: DRIZZLE_EVENT_TYPE_INTVAR

      Insert ID event

   .. py:data:: DRIZZLE_EVENT_TYPE_LOAD

      Load data from file event

   .. py:data:: DRIZZLE_EVENT_TYPE_CREATE_FILE

      Create file event

   .. py:data:: DRIZZLE_EVENT_TYPE_APPEND_BLOCK

      Append block data to a file

   .. py:data:: DRIZZLE_EVENT_TYPE_EXEC_LOAD

      Exec load event

   .. py:data:: DRIZZLE_EVENT_TYPE_DELETE_FILE

      Delete file event

   .. py:data:: DRIZZLE_EVENT_TYPE_NEW_LOAD

      New load data from file event

   .. py:data:: DRIZZLE_EVENT_TYPE_RAND

      Seeds for RAND() functions

   .. py:data:: DRIZZLE_EVENT_TYPE_USER_VAR

      A user variable

   .. py:data:: DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION

      A description of the binlog file (a replacement for
      DRIZZLE_EVENT_TYPE_START in MySQL 5.0 onwards)

   .. py:data:: DRIZZLE_EVENT_TYPE_XID

      XA Transaction ID

   .. py:data:: DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY

      Truncate file and save block data

   .. py:data:: DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY

      Execute load query event

   .. py:data:: DRIZZLE_EVENT_TYPE_TABLE_MAP

      A table map event for RBR

   .. py:data:: DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS

      RBR Write rows event for MySQL 5.1 pre-release

   .. py:data:: DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS

      RBR Update rows event for MySQL 5.1 pre-release

   .. py:data:: DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS

      RBR Delete rows event for MySQL 5.1 pre-release

   .. py:data:: DRIZZLE_EVENT_TYPE_WRITE_ROWS

      RBR Write rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_UPDATE_ROWS

      RBR Update rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_INCIDENT

      Replication incident message

   .. py:data:: DRIZZLE_EVENT_TYPE_HEARTBEAT

      Repliaction heartbeat event


