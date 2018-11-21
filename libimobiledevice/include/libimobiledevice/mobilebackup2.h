/**
 * @file libimobiledevice/mobilebackup2.h
 * @brief Backup and restore of all device data (mobilebackup2, iOS4+ only)
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

#ifndef IMOBILEBACKUP2_H
#define IMOBILEBACKUP2_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define MOBILEBACKUP2_SERVICE_NAME "com.apple.mobilebackup2"
/** @name Error Codes */
/*@{*/
#define MOBILEBACKUP2_E_SUCCESS                0
#define MOBILEBACKUP2_E_INVALID_ARG           -1
#define MOBILEBACKUP2_E_PLIST_ERROR           -2
#define MOBILEBACKUP2_E_MUX_ERROR             -3
#define MOBILEBACKUP2_E_BAD_VERSION           -4
#define MOBILEBACKUP2_E_REPLY_NOT_OK          -5
#define MOBILEBACKUP2_E_NO_COMMON_VERSION     -6

#define MOBILEBACKUP2_E_UNKNOWN_ERROR       -256
/*@}*/

/** Represents an error code. */
typedef int16_t mobilebackup2_error_t;

typedef struct mobilebackup2_client_private mobilebackup2_client_private;
typedef mobilebackup2_client_private *mobilebackup2_client_t; /**< The client handle. */

LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_client_new(idevice_t device, lockdownd_service_descriptor_t service, mobilebackup2_client_t * client);

/**
 * Starts a new mobilebackup2 service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     mobilebackup2_client_t upon successful return. Must be freed using
 *     mobilebackup2_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return MOBILEBACKUP2_E_SUCCESS on success, or an MOBILEBACKUP2_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_client_start_service(idevice_t device, mobilebackup2_client_t* client, const char* label);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_client_free(mobilebackup2_client_t client);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_send_message(mobilebackup2_client_t client, const char *message, plist_t options);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_receive_message(mobilebackup2_client_t client, plist_t *msg_plist, char **dlmessage);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_send_raw(mobilebackup2_client_t client, const char *data, uint32_t length, uint32_t *bytes);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_receive_raw(mobilebackup2_client_t client, char *data, uint32_t length, uint32_t *bytes);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_version_exchange(mobilebackup2_client_t client, double local_versions[], char count, double *remote_version);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_send_request(mobilebackup2_client_t client, const char *request, const char *target_identifier, const char *source_identifier, plist_t options);
LIBIMOBILEDEVICE_API mobilebackup2_error_t mobilebackup2_send_status_response(mobilebackup2_client_t client, int status_code, const char *status1, plist_t status2);

#ifdef __cplusplus
}
#endif

#endif
