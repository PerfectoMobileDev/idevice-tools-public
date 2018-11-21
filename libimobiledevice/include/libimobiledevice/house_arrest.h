/**
 * @file libimobiledevice/house_arrest.h
 * @brief Access app folders and their contents.
 * \internal
 *
 * Copyright (c) 2010 Nikias Bassen, All Rights Reserved.
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

#include "exports.h"

#ifndef IHOUSE_ARREST_H
#define IHOUSE_ARREST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/afc.h>

#define HOUSE_ARREST_SERVICE_NAME "com.apple.mobile.house_arrest"
/** @name Error Codes */
/*@{*/
#define HOUSE_ARREST_E_SUCCESS                0
#define HOUSE_ARREST_E_INVALID_ARG           -1
#define HOUSE_ARREST_E_PLIST_ERROR           -2
#define HOUSE_ARREST_E_CONN_FAILED           -3
#define HOUSE_ARREST_E_INVALID_MODE          -4

#define HOUSE_ARREST_E_UNKNOWN_ERROR       -256
/*@}*/

/** Represents an error code. */
typedef int16_t house_arrest_error_t;

typedef struct house_arrest_client_private house_arrest_client_private;
typedef house_arrest_client_private *house_arrest_client_t; /**< The client handle. */

/* Interface */
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_client_new(idevice_t device, lockdownd_service_descriptor_t service, house_arrest_client_t *client);

/**
 * Starts a new house_arrest service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     house_arrest_client_t upon successful return. Must be freed using
 *     house_arrest_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return HOUSE_ARREST_E_SUCCESS on success, or an HOUSE_ARREST_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_client_start_service(idevice_t device, house_arrest_client_t* client, const char* label);
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_client_free(house_arrest_client_t client);
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_send_request(house_arrest_client_t client, plist_t dict);
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_send_command(house_arrest_client_t client, const char *command, const char *appid);
LIBIMOBILEDEVICE_API house_arrest_error_t house_arrest_get_result(house_arrest_client_t client, plist_t *dict);
LIBIMOBILEDEVICE_API afc_error_t afc_client_new_from_house_arrest_client(house_arrest_client_t client, afc_client_t *afc_client);

#ifdef __cplusplus
}
#endif

#endif
