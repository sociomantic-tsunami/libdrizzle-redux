Constants
=========

Introduction
------------

Libdrizzle Redux contains a number of constants, most of what are in the form
of ENUMs.  All ENUMs are typedef'd so no need to use the 'enum' keyword.

Library
-------

.. c:type:: drizzle_verbose_t

   An ENUM of the verbosity for the library

   .. py:data:: DRIZZLE_VERBOSE_NEVER

      Completely silent

   .. py:data:: DRIZZLE_VERBOSE_FATAL

      Fatal errors only

   .. py:data:: DRIZZLE_VERBOSE_ERROR

      All errors

   .. py:data:: DRIZZLE_VERBOSE_INFO

      Information messages and errors

   .. py:data:: DRIZZLE_VERBOSE_DEBUG

      Debugging messages and errors

   .. py:data:: DRIZZLE_VERBOSE_CRAZY

      Everything

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

Connection
----------

.. c:type:: drizzle_options_t

   An ENUM of connection options intended to be used in a bit field

   .. py:data:: DRIZZLE_CON_NONE

      No option set

   .. py:data:: DRIZZLE_CON_NON_BLOCKING

      Connect in non-blocking mode

   .. py:data:: DRIZZLE_CON_RAW_PACKET

      Raw packet data mode (for low-level clients)

   .. py:data:: DRIZZLE_CON_RAW_SCRAMBLE

      Raw auth scramble data (for low-level clients)

   .. py:data:: DRIZZLE_CON_READY

      Connection is ready

   .. py:data:: DRIZZLE_CON_NO_RESULT_READ

      Don't read result data from buffer (advanced feature, use with caution)

   .. py:data:: DRIZZLE_CON_IO_READY
   
      Used for event handling

   .. py:data:: DRIZZLE_CON_LISTEN

      Unused

   .. py:data:: DRIZZLE_CON_FOUND_ROWS

      Enable the FOUND_ROWS functionality

   .. py:data:: DRIZZLE_CON_INTERACTIVE

      This is an interactive client

   .. py:data:: DRIZZLE_CON_MULTI_STATEMENTS

      Multi-statement support

   .. py:data:: DRIZZLE_CON_AUTH_PLUGIN

      Authentiaction plugin support

.. c:type:: drizzle_charset_t

   Character set ID, an alias for uint8_t

.. c:type:: drizzle_status_t

   An ENUM of connection statuses intended to be used in a bit field

   .. py:data:: DRIZZLE_CON_STATUS_NONE

      No status set

   .. py:data:: DRIZZLE_CON_STATUS_IN_TRANS

      In a transaction

   .. py:data:: DRIZZLE_CON_STATUS_AUTOCOMMIT

      Autocommit is enabled

   .. py:data:: DRIZZLE_CON_STATUS_MORE_RESULTS_EXISTS

      There are more result sets available

   .. py:data:: DRIZZLE_CON_STATUS_QUERY_NO_GOOD_INDEX_USED

      No good index couldn't be used

   .. py:data:: DRIZZLE_CON_STATUS_QUERY_NO_INDEX_USED

      No index was used

   .. py:data:: DRIZZLE_CON_STATUS_CURSOR_EXISTS

      A cursor is available

   .. py:data:: DRIZZLE_CON_STATUS_LAST_ROW_SENT

      The last row has been sent to the client

   .. py:data:: DRIZZLE_CON_STATUS_DB_DROPPED

      The database has been dropped

   .. py:data:: DRIZZLE_CON_STATUS_NO_BACKSLASH_ESCAPES

      NO_BACKSLASH_ESCAPES SQL mode set

   .. py:data:: DRIZZLE_CON_STATUS_QUERY_WAS_SLOW

      Query hit the slow query timeout

.. c:type:: drizzle_capabilities_t

   An ENUM of connection capabilities intended to be used in a bit field

   .. py:data:: DRIZZLE_CAPABILITIES_NONE

      No capabilities set

   .. py:data:: DRIZZLE_CAPABILITIES_LONG_PASSWORD

      Long password support

   .. py:data:: DRIZZLE_CAPABILITIES_FOUND_ROWS

      FOUND_ROWS support

   .. py:data:: DRIZZLE_CAPABILITIES_LONG_FLAG

      Get all column flags

   .. py:data:: DRIZZLE_CAPABILITIES_IGNORE_SPACE

      Ignore spaces before open brackets

   .. py:data:: DRIZZLE_CAPABILITIES_CONNECT_WITH_DB

      A database can be specified upon connect

   .. py:data:: DRIZZLE_CAPABILITIES_NO_SCHEMA

      Disable access to database.table.column way of accessing things

   .. py:data:: DRIZZLE_CAPABILITIES_COMPRESS

      Enable compression protocol

   .. py:data:: DRIZZLE_CAPABILITIES_ODBC

      An ODBC client

   .. py:data:: DRIZZLE_CAPABILITIES_LOCAL_FILES

      Enables LOAD DATA LOCAL

   .. py:data:: DRIZZLE_CAPABILITIES_PROTOCOL_41

      MySQL 4.1 and higher protocol

   .. py:data:: DRIZZLE_CAPABILITIES_INTERACTIVE

      An interactive client

   .. py:data:: DRIZZLE_CAPABILITIES_SSL

      Use SSL

   .. py:data:: DRIZZLE_CAPABILITIES_IGNORE_SIGPIPE

      Ignore sigpipe

   .. py:data:: DRIZZLE_CAPABILITIES_TRANSACTIONS

      Client understands transactions

   .. py:data:: DRIZZLE_CAPABILITIES_RESERVED

      Unused

   .. py:data:: DRIZZLE_CAPABILITIES_SECURE_CONNECTION

      MySQL 4.1 and higher authentication

   .. py:data:: DRIZZLE_CAPABILITIES_MULTI_STATEMENTS

      Enable multiple statement support

   .. py:data:: DRIZZLE_CAPABILITIES_MULTI_RESULTS

      Enable multiple result sets

   .. py:data:: DRIZZLE_CAPABILITIES_PS_MULTI_RESULTS

   .. py:data:: DRIZZLE_CAPABILITIES_PLUGIN_AUTH

      Enable plugin authentication

   .. py:data:: DRIZZLE_CAPABILITIES_SSL_VERIFY_SERVER_CERT

      Verify SSL cert

   .. py:data:: DRIZZLE_CAPABILITIES_REMEBER_OPTIONS

   .. py:data:: DRIZZLE_CAPABILITIES_CLIENT

      Enables the following:
      :py:const:`DRIZZLE_CAPABILITIES_LONG_PASSWORD`,
      :py:const:`DRIZZLE_CAPABILITIES_FOUND_ROWS`,
      :py:const:`DRIZZLE_CAPABILITIES_LONG_FLAG`,
      :py:const:`DRIZZLE_CAPABILITIES_CONNECT_WITH_DB`,
      :py:const:`DRIZZLE_CAPABILITIES_PLUGIN_AUTH`,
      :py:const:`DRIZZLE_CAPABILITIES_TRANSACTIONS`,
      :py:const:`DRIZZLE_CAPABILITIES_PROTOCOL_41`,
      :py:const:`DRIZZLE_CAPABILITIES_SECURE_CONNECTION`

Query
-----

.. c:type:: drizzle_field_t

   Field data (an alias for :c:type:`char*`)

.. c:type:: drizzle_row_t

   Row data (an array of :c:type:`drizzle_field_t`)

.. c:type:: drizzle_column_t

   An ENUM of column types

   .. py:data:: DRIZZLE_COLUMN_TYPE_DECIMAL

      An old style decimal type

   .. py:data:: DRIZZLE_COLUMN_TYPE_TINY

      A tiny int

   .. py:data:: DRIZZLE_COLUMN_TYPE_SHORT

      A short int

   .. py:data:: DRIZZLE_COLUMN_TYPE_LONG

      A long int

   .. py:data:: DRIZZLE_COLUMN_TYPE_FLOAT

      A float

   .. py:data:: DRIZZLE_COLUMN_TYPE_DOUBLE

      A double

   .. py:data:: DRIZZLE_COLUMN_TYPE_NULL

      A NULL

   .. py:data:: DRIZZLE_COLUMN_TYPE_TIMESTAMP

      A timestamp

   .. py:data:: DRIZZLE_COLUMN_TYPE_LONGLONG

      A bigint

   .. py:data:: DRIZZLE_COLUMN_TYPE_INT24
   .. py:data:: DRIZZLE_COLUMN_TYPE_DATE
   .. py:data:: DRIZZLE_COLUMN_TYPE_TIME
   .. py:data:: DRIZZLE_COLUMN_TYPE_DATETIME
   .. py:data:: DRIZZLE_COLUMN_TYPE_YEAR
   .. py:data:: DRIZZLE_COLUMN_TYPE_NEWDATE
   .. py:data:: DRIZZLE_COLUMN_TYPE_VARCHAR
   .. py:data:: DRIZZLE_COLUMN_TYPE_BIT
   .. py:data:: DRIZZLE_COLUMN_TYPE_NEWDECIMAL
   .. py:data:: DRIZZLE_COLUMN_TYPE_ENUM
   .. py:data:: DRIZZLE_COLUMN_TYPE_SET
   .. py:data:: DRIZZLE_COLUMN_TYPE_TINY_BLOB
   .. py:data:: DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB
   .. py:data:: DRIZZLE_COLUMN_TYPE_LONG_BLOB
   .. py:data:: DRIZZLE_COLUMN_TYPE_BLOB
   .. py:data:: DRIZZLE_COLUMN_TYPE_VAR_STRING

      Text column type

   .. py:data:: DRIZZLE_COLUMN_TYPE_STRING
   .. py:data:: DRIZZLE_COLUMN_TYPE_GEOMETRY

.. c:type:: drizzle_column_flags_t

   An ENUM of column flags intended to be used in a bit field

   .. py:data:: DRIZZLE_COLUMN_FLAGS_NONE

      No flags set

   .. py:data:: DRIZZLE_COLUMN_FLAGS_NOT_NULL

      Column is not NULL

   .. py:data:: DRIZZLE_COLUMN_FLAGS_PRI_KEY

      Column is a primary key

   .. py:data:: DRIZZLE_COLUMN_FLAGS_UNIQUE_KEY

      Column is a unique key

   .. py:data:: DRIZZLE_COLUMN_FLAGS_MULTIPLE_KEY

      Column is part of a multi-part key

   .. py:data:: DRIZZLE_COLUMN_FLAGS_BLOB

      Column is a blob

   .. py:data:: DRIZZLE_COLUMN_FLAGS_UNSIGNED

      Column in unsigned

   .. py:data:: DRIZZLE_COLUMN_FLAGS_ZEROFILL

      Column has a zerofill

   .. py:data:: DRIZZLE_COLUMN_FLAGS_BINARY
   .. py:data:: DRIZZLE_COLUMN_FLAGS_ENUM

      Column is an ENUM

   .. py:data:: DRIZZLE_COLUMN_FLAGS_AUTO_INCREMENT

      Column has auto increment

   .. py:data:: DRIZZLE_COLUMN_FLAGS_TIMESTAMP

      Column in a timestamp

   .. py:data:: DRIZZLE_COLUMN_FLAGS_SET

      Column is a SET data type

   .. py:data:: DRIZZLE_COLUMN_FLAGS_NO_DEFAULT_VALUE

      Column has no default value

   .. py:data:: DRIZZLE_COLUMN_FLAGS_ON_UPDATE_NOW

      Column has on update now timestamp

   .. py:data:: DRIZZLE_COLUMN_FLAGS_PART_KEY

      Column is part of a key

   .. py:data:: DRIZZLE_COLUMN_FLAGS_NUM

      Column is a number

      .. note::
         Group and num are the same flag

   .. py:data:: DRIZZLE_COLUMN_FLAGS_GROUP

      .. note::
         Group and num are the same flag

   .. py:data:: DRIZZLE_COLUMN_FLAGS_UNIQUE
   .. py:data:: DRIZZLE_COLUMN_FLAGS_BINCMP
   .. py:data:: DRIZZLE_COLUMN_FLAGS_GET_FIXED_FIELDS
   .. py:data:: DRIZZLE_COLUMN_FLAGS_IN_PART_FUNC
   .. py:data:: DRIZZLE_COLUMN_FLAGS_IN_ADD_INDEX
   .. py:data:: DRIZZLE_COLUMN_FLAGS_RENAMED


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


