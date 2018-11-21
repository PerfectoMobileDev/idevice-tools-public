/**
 * @file libimobiledevice/file_relay.h
 * @brief Retrieve compressed CPIO archives.
 * \internal
 *
 * Copyright (c) 2010 Nikias Bassen All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef IFILE_RELAY_H
#define IFILE_RELAY_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define FILE_RELAY_SERVICE_NAME "com.apple.mobile.file_relay"
/** @name Error Codes */
/*@{*/
#define FILE_RELAY_E_SUCCESS                0
#define FILE_RELAY_E_INVALID_ARG           -1
#define FILE_RELAY_E_PLIST_ERROR           -2
#define FILE_RELAY_E_MUX_ERROR             -3
#define FILE_RELAY_E_INVALID_SOURCE        -4
#define FILE_RELAY_E_STAGING_EMPTY         -5
#define FILE_RELAY_E_PERMISSION_DENIED	   -6
#define FILE_RELAY_E_UNKNOWN_ERROR         -256
/*@}*/

/** Represents an error code. */
typedef int16_t file_relay_error_t;

typedef struct file_relay_client_private file_relay_client_private;
typedef file_relay_client_private *file_relay_client_t; /**< The client handle. */

LIBIMOBILEDEVICE_API file_relay_error_t file_relay_client_new(idevice_t device, lockdownd_service_descriptor_t service, file_relay_client_t *client);

/**
 * Starts a new file_relay service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     file_relay_client_t upon successful return. Must be freed using
 *     file_relay_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return FILE_RELAY_E_SUCCESS on success, or an FILE_RELAY_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API file_relay_error_t file_relay_client_start_service(idevice_t device, file_relay_client_t* client, const char* label);
LIBIMOBILEDEVICE_API file_relay_error_t file_relay_client_free(file_relay_client_t client);
LIBIMOBILEDEVICE_API file_relay_error_t file_relay_request_sources(file_relay_client_t client, const char **sources, idevice_connection_t *connection);

/**
 * Request data for the given sources. Calls file_relay_request_sources_timeout() with
 * a timeout of 60000 milliseconds (60 seconds).
 *
 * @param client The connected file_relay client.
 * @param sources A NULL-terminated list of sources to retrieve.
 *     Valid sources are:
 *     - AppleSupport
 *     - Network
 *     - VPN
 *     - WiFi
 *     - UserDatabases
 *     - CrashReporter
 *     - tmp
 *     - SystemConfiguration
 * @param connection The connection that has to be used for receiving the
 *     data using idevice_connection_receive(). The connection will be closed
 *     automatically by the device, but use file_relay_client_free() to clean
 *     up properly.
 *
 * @note WARNING: Don't call this function without reading the data afterwards.
 *     A directory mobile_file_relay.XXXX used for creating the archive will
 *     remain in the /tmp directory otherwise.
 *
 * @return FILE_RELAY_E_SUCCESS on succes, FILE_RELAY_E_INVALID_ARG when one or
 *     more parameters are invalid, FILE_RELAY_E_MUX_ERROR if a communication
 *     error occurs, FILE_RELAY_E_PLIST_ERROR when the received result is NULL
 *     or is not a valid plist, FILE_RELAY_E_INVALID_SOURCE if one or more
 *     sources are invalid, FILE_RELAY_E_STAGING_EMPTY if no data is available
 *     for the given sources, or FILE_RELAY_E_UNKNOWN_ERROR otherwise.
 */
LIBIMOBILEDEVICE_API file_relay_error_t file_relay_request_sources_timeout(file_relay_client_t client, const char **sources, idevice_connection_t *connection, unsigned int timeout);

#ifdef __cplusplus
}
#endif

#endif
