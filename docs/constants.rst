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


