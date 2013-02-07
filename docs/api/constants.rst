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

   .. py:data:: DRIZZLE_RETURN_STMT_ERROR

      A prepared statment error has occurred

   .. py:data:: DRIZZLE_RETURN_BINLOG_CRC

      A checksum error has occurred in a MySQL 5.6 binlog

   .. py:data:: DRIZZLE_RETURN_TRUNCATED

      The result has been truncated

   .. py:data:: DRIZZLE_RETURN_INVALID_CONVERSION

      The data type cannot be converted into the requested type

   .. py:data:: DRIZZLE_RETURN_NOT_FOUND

      The requested column was not found

Connection
----------

.. c:type:: drizzle_charset_t

   An ENUM of the possible character set with colation ID

   .. py:data:: DRIZZLE_CHARSET_BIG5_CHINESE_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN2_CZECH_CS

   .. py:data:: DRIZZLE_CHARSET_DEC8_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_CP850_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN1_GERMAN1_CI

   .. py:data:: DRIZZLE_CHARSET_HP8_ENGLISH_CI

   .. py:data:: DRIZZLE_CHARSET_KOI8R_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN1_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN2_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_SWE7_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_ASCII_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UJIS_JAPANESE_CI

   .. py:data:: DRIZZLE_CHARSET_SJIS_JAPANESE_CI

   .. py:data:: DRIZZLE_CHARSET_CP1251_BULGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN1_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_HEBREW_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_TIS620_THAI_CI

   .. py:data:: DRIZZLE_CHARSET_EUCKR_KOREAN_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN7_ESTONIAN_CS

   .. py:data:: DRIZZLE_CHARSET_LATIN2_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_KOI8U_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP1251_UKRAINIAN_CI

   .. py:data:: DRIZZLE_CHARSET_GB2312_CHINESE_CI

   .. py:data:: DRIZZLE_CHARSET_GREEK_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP1250_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN2_CROATIAN_CI

   .. py:data:: DRIZZLE_CHARSET_GBK_CHINESE_CI

   .. py:data:: DRIZZLE_CHARSET_CP1257_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN5_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN1_GERMAN2_CI

   .. py:data:: DRIZZLE_CHARSET_ARMSCII8_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP1250_CZECH_CS

   .. py:data:: DRIZZLE_CHARSET_UCS2_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP866_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_KEYBCS2_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_MACCE_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_MACROMAN_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP852_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN7_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN7_GENERAL_CS

   .. py:data:: DRIZZLE_CHARSET_MACCE_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1250_CROATIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN1_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN1_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_LATIN1_GENERAL_CS

   .. py:data:: DRIZZLE_CHARSET_CP1251_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1251_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP1251_GENERAL_CS

   .. py:data:: DRIZZLE_CHARSET_MACROMAN_BIN

   .. py:data:: DRIZZLE_CHARSET_UTF16_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1256_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_CP1257_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1257_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_BIN

   .. py:data:: DRIZZLE_CHARSET_BINARY

   .. py:data:: DRIZZLE_CHARSET_ARMSCII8_BIN

   .. py:data:: DRIZZLE_CHARSET_ASCII_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1250_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1256_BIN

   .. py:data:: DRIZZLE_CHARSET_CP866_BIN

   .. py:data:: DRIZZLE_CHARSET_DEC8_BIN

   .. py:data:: DRIZZLE_CHARSET_GREEK_BIN

   .. py:data:: DRIZZLE_CHARSET_HEBREW_BIN

   .. py:data:: DRIZZLE_CHARSET_HP8_BIN

   .. py:data:: DRIZZLE_CHARSET_KEYBCS2_BIN

   .. py:data:: DRIZZLE_CHARSET_KOI8R_BIN

   .. py:data:: DRIZZLE_CHARSET_KOI8U_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN2_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN5_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN7_BIN

   .. py:data:: DRIZZLE_CHARSET_CP850_BIN

   .. py:data:: DRIZZLE_CHARSET_CP852_BIN

   .. py:data:: DRIZZLE_CHARSET_SWE7_BIN

   .. py:data:: DRIZZLE_CHARSET_UTF8_BIN

   .. py:data:: DRIZZLE_CHARSET_BIG5_BIN

   .. py:data:: DRIZZLE_CHARSET_EUCKR_BIN

   .. py:data:: DRIZZLE_CHARSET_GB2312_BIN

   .. py:data:: DRIZZLE_CHARSET_GBK_BIN

   .. py:data:: DRIZZLE_CHARSET_SJIS_BIN

   .. py:data:: DRIZZLE_CHARSET_TIS620_BIN

   .. py:data:: DRIZZLE_CHARSET_UCS2_BIN

   .. py:data:: DRIZZLE_CHARSET_UJIS_BIN

   .. py:data:: DRIZZLE_CHARSET_GEOSTD8_GENERAL_CI

   .. py:data:: DRIZZLE_CHARSET_GEOSTD8_BIN

   .. py:data:: DRIZZLE_CHARSET_LATIN1_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_CP932_JAPANESE_CI

   .. py:data:: DRIZZLE_CHARSET_CP932_BIN

   .. py:data:: DRIZZLE_CHARSET_EUCJPMS_JAPANESE_CI

   .. py:data:: DRIZZLE_CHARSET_EUCJPMS_BIN

   .. py:data:: DRIZZLE_CHARSET_CP1250_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_UNICODE_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_ICELANDIC_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_LATVIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_ROMANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SLOVENIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_ESTONIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_CZECH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SLOVAK_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SPANISH2_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_ROMAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_PERSIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_ESPERANTO_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF16_SINHALA_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_UNICODE_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_ICELANDIC_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_LATVIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_ROMANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SLOVENIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_ESTONIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_CZECH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SLOVAK_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SPANISH2_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_ROMAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_PERSIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_ESPERANTO_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_SINHALA_CI

   .. py:data:: DRIZZLE_CHARSET_UCS2_GENERAL_MYSQL500_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_UNICODE_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_ICELANDIC_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_LATVIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_ROMANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SLOVENIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_ESTONIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_CZECH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SLOVAK_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SPANISH2_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_ROMAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_PERSIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_ESPERANTO_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF32_SINHALA_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_UNICODE_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_ICELANDIC_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_LATVIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_ROMANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SLOVENIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_ESTONIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_CZECH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SLOVAK_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SPANISH2_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_ROMAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_PERSIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_ESPERANTO_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_SINHALA_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8_GENERAL_MYSQL500_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_UNICODE_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_ICELANDIC_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_LATVIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_ROMANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SLOVENIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_POLISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_ESTONIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SPANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SWEDISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_TURKISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_CZECH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_DANISH_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_LITHUANIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SLOVAK_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SPANISH2_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_ROMAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_PERSIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_ESPERANTO_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_HUNGARIAN_CI

   .. py:data:: DRIZZLE_CHARSET_UTF8MB4_SINHALA_CI


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

.. c:type:: drizzle_column_type_t

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

Prepared Statement
------------------

.. c:type:: drizzle_stmt_state_t

   An internal state for prepared statements

.. c:type:: drizzle_bind_options_t

   An ENUM of prepared statement element options intended to be used in a bitmask
   
   .. py:data:: DRIZZLE_BIND_OPTION_NONE

      Empty options

   .. py:data:: DRIZZLE_BIND_OPTION_NULL

      Element is a NULL

   .. py:data:: DRIZZLE_BIND_OPTION_UNSIGNED

      Element is an unsigned integer

   .. py:data:: DRIZZLE_BIND_OPTION_TRUNCATED

      Element has been truncated

   .. py:data:: DRIZZLE_BIND_OPTION_LONG_DATA

      Element is to be sent using :c:func:`drizzle_stmt_send_long_data`

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

   .. py:data:: DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS

      RBR Write rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS

      RBR Update rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS

      RBR Delete rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_INCIDENT

      Replication incident message

   .. py:data:: DRIZZLE_EVENT_TYPE_HEARTBEAT

      Repliaction heartbeat event

   .. py:data:: DRIZZLE_EVENT_TYPE_IGNORABLE

   .. py:data:: DRIZZLE_EVENT_TYPE_ROWS_QUERY

   .. py:data:: DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS

      A MySQL 5.6 RBR Write rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS

      A MySQL 5.6 RBR Update rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS

      A MySQL 5.6 RBR Delete rows event

   .. py:data:: DRIZZLE_EVENT_TYPE_GTID

   .. py:data:: DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID

   .. py:data:: DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS

