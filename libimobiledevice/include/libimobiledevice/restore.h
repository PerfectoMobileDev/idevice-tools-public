/**
 * @file libimobiledevice/restore.h
 * @brief Initiate restore process or reboot device.
 * @note This service is only available if the device is in restore mode.
 * \internal
 *
 * Copyright (c) 2010 Joshua Hill. All Rights Reserved.
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

#ifndef IRESTORE_H
#define IRESTORE_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>

/** @name Error Codes */
/*@{*/
#define RESTORE_E_SUCCESS                     0
#define RESTORE_E_INVALID_ARG                -1
#define RESTORE_E_INVALID_CONF               -2
#define RESTORE_E_PLIST_ERROR                -3
#define RESTORE_E_DICT_ERROR                 -4
#define RESTORE_E_NOT_ENOUGH_DATA            -5
#define RESTORE_E_MUX_ERROR                  -6
#define RESTORE_E_START_RESTORE_FAILED       -7
#define RESTORE_E_DEVICE_ERROR				 -8
#define RESTORE_E_UNKNOWN_ERROR            -256
/*@}*/

/** Represents an error code. */
typedef int16_t restored_error_t;

typedef struct restored_client_private restored_client_private;
typedef restored_client_private *restored_client_t; /**< The client handle. */

/* Interface */
LIBIMOBILEDEVICE_API restored_error_t restored_client_new(idevice_t device, restored_client_t *client, const char *label);
LIBIMOBILEDEVICE_API restored_error_t restored_client_free(restored_client_t client);
LIBIMOBILEDEVICE_API restored_error_t restored_query_type(restored_client_t client, char **type, uint64_t *version);
LIBIMOBILEDEVICE_API restored_error_t restored_query_value(restored_client_t client, const char *key, plist_t *value);
LIBIMOBILEDEVICE_API restored_error_t restored_get_value(restored_client_t client, const char *key, plist_t *value);
LIBIMOBILEDEVICE_API restored_error_t restored_send(restored_client_t client, plist_t plist);
LIBIMOBILEDEVICE_API restored_error_t restored_receive(restored_client_t client, plist_t *plist);
LIBIMOBILEDEVICE_API restored_error_t restored_goodbye(restored_client_t client);
LIBIMOBILEDEVICE_API restored_error_t restored_start_restore(restored_client_t client, plist_t options, uint64_t version);
LIBIMOBILEDEVICE_API restored_error_t restored_reboot(restored_client_t client);

/* Helper */
LIBIMOBILEDEVICE_API void restored_client_set_label(restored_client_t client, const char *label);

#ifdef __cplusplus
}
#endif

#endif
