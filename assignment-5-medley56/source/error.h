/**
  * @file error.h
  * @brief Error codes description
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-2.0-or-later
  *
  * Copyright (C) 2010-2023 Oryx Embedded SARL. All rights reserved.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software Foundation,
  * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  *
  * @author Oryx Embedded SARL (www.oryx-embedded.com)
  * @version 2.2.4
  **/

 #ifndef _ERROR_H
 #define _ERROR_H

 //C++ guard
 #ifdef __cplusplus
 extern "C" {
 #endif


 /**
  * @brief Error codes
  **/

 typedef enum
 {
    NO_ERROR = 0,                    ///<Success
    ERROR_FAILURE = 1,               ///<Generic error code

    ERROR_INVALID_PARAMETER,         ///<Invalid parameter
    ERROR_PARAMETER_OUT_OF_RANGE,    ///<Specified parameter is out of range

    ERROR_BAD_CRC,
    ERROR_BAD_BLOCK,
    ERROR_INVALID_RECIPIENT,         ///<Invalid recipient
    ERROR_INVALID_INTERFACE,         ///<Invalid interface
    ERROR_INVALID_ENDPOINT,          ///<Invalid endpoint
    ERROR_INVALID_ALT_SETTING,       ///<Alternate setting does not exist
    ERROR_UNSUPPORTED_REQUEST,       ///<Unsupported request
    ERROR_UNSUPPORTED_CONFIGURATION, ///<Unsupported configuration
    ERROR_UNSUPPORTED_FEATURE,       ///<Unsupported feature
    ERROR_ENDPOINT_BUSY,             ///<Endpoint already in use
    ERROR_USB_RESET,
    ERROR_ABORTED,

    ERROR_OUT_OF_MEMORY = 100,
    ERROR_OUT_OF_RESOURCES,
    ERROR_INVALID_REQUEST,
    ERROR_NOT_IMPLEMENTED,
    ERROR_VERSION_NOT_SUPPORTED,
    ERROR_INVALID_SYNTAX,
    ERROR_AUTHENTICATION_FAILED,
    ERROR_UNEXPECTED_RESPONSE,
    ERROR_INVALID_RESPONSE,
    ERROR_UNEXPECTED_VALUE,
    ERROR_WAIT_CANCELED,

    ERROR_OPEN_FAILED = 200,
    ERROR_CONNECTION_FAILED,
    ERROR_CONNECTION_REFUSED,
    ERROR_CONNECTION_CLOSING,
    ERROR_CONNECTION_RESET,
    ERROR_NOT_CONNECTED,
    ERROR_ALREADY_CLOSED,
    ERROR_ALREADY_CONNECTED,
    ERROR_INVALID_SOCKET,
    ERROR_PROTOCOL_UNREACHABLE,
    ERROR_PORT_UNREACHABLE,
    ERROR_INVALID_FRAME,
    ERROR_INVALID_HEADER,
    ERROR_WRONG_CHECKSUM,
    ERROR_WRONG_IDENTIFIER,
    ERROR_WRONG_CLIENT_ID,
    ERROR_WRONG_SERVER_ID,
    ERROR_WRONG_COOKIE,
    ERROR_NO_RESPONSE,
    ERROR_RECEIVE_QUEUE_FULL,
    ERROR_TIMEOUT,
    ERROR_WOULD_BLOCK,
    ERROR_INVALID_NAME,
    ERROR_INVALID_OPTION,
    ERROR_UNEXPECTED_STATE,
    ERROR_INVALID_COMMAND,
    ERROR_INVALID_PROTOCOL,
    ERROR_INVALID_STATUS,
    ERROR_INVALID_ADDRESS,
    ERROR_INVALID_PORT,
    ERROR_INVALID_MESSAGE,
    ERROR_INVALID_KEY,
    ERROR_INVALID_KEY_LENGTH,
    ERROR_INVALID_EPOCH,
    ERROR_INVALID_SEQUENCE_NUMBER,
    ERROR_INVALID_CHARACTER,
    ERROR_INVALID_LENGTH,
    ERROR_INVALID_PADDING,
    ERROR_INVALID_MAC,
    ERROR_INVALID_TAG,
    ERROR_INVALID_TYPE,
    ERROR_INVALID_VALUE,
    ERROR_INVALID_CLASS,
    ERROR_INVALID_VERSION,
    ERROR_INVALID_PIN_CODE,
    ERROR_WRONG_LENGTH,
    ERROR_WRONG_TYPE,
    ERROR_WRONG_ENCODING,
    ERROR_WRONG_VALUE,
    ERROR_INCONSISTENT_VALUE,
    ERROR_UNSUPPORTED_TYPE,
    ERROR_UNSUPPORTED_ALGO,
    ERROR_UNSUPPORTED_CIPHER_SUITE,
    ERROR_UNSUPPORTED_CIPHER_MODE,
    ERROR_UNSUPPORTED_CIPHER_ALGO,
    ERROR_UNSUPPORTED_HASH_ALGO,
    ERROR_UNSUPPORTED_KEY_EXCH_ALGO,
    ERROR_UNSUPPORTED_SIGNATURE_ALGO,
    ERROR_UNSUPPORTED_ELLIPTIC_CURVE,
    ERROR_INVALID_SIGNATURE_ALGO,
    ERROR_CERTIFICATE_REQUIRED,
    ERROR_MESSAGE_TOO_LONG,
    ERROR_OUT_OF_RANGE,
    ERROR_MESSAGE_DISCARDED,

    ERROR_INVALID_PACKET,
    ERROR_BUFFER_EMPTY,
    ERROR_BUFFER_OVERFLOW,
    ERROR_BUFFER_UNDERFLOW,

    ERROR_INVALID_RESOURCE,
    ERROR_INVALID_PATH,
    ERROR_NOT_FOUND,
    ERROR_ACCESS_DENIED,
    ERROR_NOT_WRITABLE,
    ERROR_AUTH_REQUIRED,

    ERROR_TRANSMITTER_BUSY,
    ERROR_NO_RUNNING,

    ERROR_INVALID_FILE = 300,
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_OPENING_FAILED,
    ERROR_FILE_READING_FAILED,
    ERROR_END_OF_FILE,
    ERROR_UNEXPECTED_END_OF_FILE,
    ERROR_UNKNOWN_FILE_FORMAT,

    ERROR_INVALID_DIRECTORY,
    ERROR_DIRECTORY_NOT_FOUND,

    ERROR_FILE_SYSTEM_NOT_SUPPORTED = 400,
    ERROR_UNKNOWN_FILE_SYSTEM,
    ERROR_INVALID_FILE_SYSTEM,
    ERROR_INVALID_BOOT_SECTOR_SIGNATURE,
    ERROR_INVALID_SECTOR_SIZE,
    ERROR_INVALID_CLUSTER_SIZE,
    ERROR_INVALID_FILE_RECORD_SIZE,
    ERROR_INVALID_INDEX_BUFFER_SIZE,
    ERROR_INVALID_VOLUME_DESCRIPTOR_SIGNATURE,
    ERROR_INVALID_VOLUME_DESCRIPTOR,
    ERROR_INVALID_FILE_RECORD,
    ERROR_INVALID_INDEX_BUFFER,
    ERROR_INVALID_DATA_RUNS,
    ERROR_WRONG_TAG_IDENTIFIER,
    ERROR_WRONG_TAG_CHECKSUM,
    ERROR_WRONG_MAGIC_NUMBER,
    ERROR_WRONG_SEQUENCE_NUMBER,
    ERROR_DESCRIPTOR_NOT_FOUND,
    ERROR_ATTRIBUTE_NOT_FOUND,
    ERROR_RESIDENT_ATTRIBUTE,
    ERROR_NOT_RESIDENT_ATTRIBUTE,
    ERROR_INVALID_SUPER_BLOCK,
    ERROR_INVALID_SUPER_BLOCK_SIGNATURE,
    ERROR_INVALID_BLOCK_SIZE,
    ERROR_UNSUPPORTED_REVISION_LEVEL,
    ERROR_INVALID_INODE_SIZE,
    ERROR_INODE_NOT_FOUND,

    ERROR_UNEXPECTED_MESSAGE = 500,

    ERROR_URL_TOO_LONG,
    ERROR_QUERY_STRING_TOO_LONG,

    ERROR_NO_ADDRESS,
    ERROR_NO_BINDING,
    ERROR_NOT_ON_LINK,
    ERROR_USE_MULTICAST,
    ERROR_NAK_RECEIVED,
    ERROR_EXCEPTION_RECEIVED,

    ERROR_NO_CARRIER,

    ERROR_INVALID_LEVEL,
    ERROR_WRONG_STATE,
    ERROR_END_OF_STREAM,
    ERROR_LINK_DOWN,
    ERROR_INVALID_OPTION_LENGTH,
    ERROR_IN_PROGRESS,

    ERROR_NO_ACK,
    ERROR_INVALID_METADATA,
    ERROR_NOT_CONFIGURED,
    ERROR_NAME_RESOLUTION_FAILED,
    ERROR_NO_ROUTE,

    ERROR_WRITE_FAILED,
    ERROR_READ_FAILED,
    ERROR_UPLOAD_FAILED,
    ERROR_READ_ONLY_ACCESS,

    ERROR_INVALID_SIGNATURE,
    ERROR_INVALID_TICKET,
    ERROR_NO_TICKET,

    ERROR_BAD_RECORD_MAC,
    ERROR_RECORD_OVERFLOW,
    ERROR_HANDSHAKE_FAILED,
    ERROR_NO_CERTIFICATE,
    ERROR_BAD_CERTIFICATE,
    ERROR_UNSUPPORTED_CERTIFICATE,
    ERROR_UNKNOWN_CERTIFICATE,
    ERROR_CERTIFICATE_EXPIRED,
    ERROR_CERTIFICATE_REVOKED,
    ERROR_UNKNOWN_CA,
    ERROR_DECODING_FAILED,
    ERROR_DECRYPTION_FAILED,
    ERROR_ILLEGAL_PARAMETER,
    ERROR_MISSING_EXTENSION,
    ERROR_UNSUPPORTED_EXTENSION,
    ERROR_INAPPROPRIATE_FALLBACK,
    ERROR_NO_APPLICATION_PROTOCOL,

    ERROR_MORE_DATA_REQUIRED,
    ERROR_TLS_NOT_SUPPORTED,
    ERROR_PRNG_NOT_READY,
    ERROR_SERVICE_CLOSING,
    ERROR_INVALID_TIMESTAMP,
    ERROR_NO_DNS_SERVER,

    ERROR_OBJECT_NOT_FOUND,
    ERROR_INSTANCE_NOT_FOUND,
    ERROR_ADDRESS_NOT_FOUND,

    ERROR_UNKNOWN_IDENTITY,
    ERROR_UNKNOWN_ENGINE_ID,
    ERROR_UNKNOWN_USER_NAME,
    ERROR_UNKNOWN_CONTEXT,
    ERROR_UNAVAILABLE_CONTEXT,
    ERROR_UNSUPPORTED_SECURITY_LEVEL,
    ERROR_NOT_IN_TIME_WINDOW,
    ERROR_AUTHORIZATION_FAILED,

    ERROR_INVALID_FUNCTION_CODE,
    ERROR_DEVICE_BUSY,

    ERROR_REQUEST_REJECTED,

    ERROR_INVALID_CHANNEL,
    ERROR_INVALID_GROUP,
    ERROR_UNKNOWN_SERVICE,
    ERROR_UNKNOWN_REQUEST,
    ERROR_FLOW_CONTROL,

    ERROR_INVALID_PASSWORD,
    ERROR_INVALID_HANDLE,
    ERROR_BAD_NONCE,
    ERROR_UNEXPECTED_STATUS,
    ERROR_RESPONSE_TOO_LARGE,

    ERROR_INVALID_SESSION,
    ERROR_TICKET_EXPIRED,

    ERROR_INVALID_ENTRY,
    ERROR_TABLE_FULL,
    ERROR_END_OF_TABLE,

    ERROR_ALREADY_RUNNING,
    ERROR_UNKOWN_KEY,
    ERROR_UNKNOWN_TYPE,
    ERROR_UNSUPPORTED_OPTION,
    ERROR_INVALID_SPI,
    ERROR_RETRY,
    ERROR_POLICY_FAILURE,
    ERROR_INVALID_PROPOSAL,
    ERROR_INVALID_SELECTOR,

    ERROR_NO_MATCH,
    ERROR_PARTIAL_MATCH
 } error_t;

 //C++ guard
 #ifdef __cplusplus
 }
 #endif

 #endif
