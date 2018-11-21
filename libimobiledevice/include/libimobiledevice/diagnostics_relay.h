/**
 * @file libimobiledevice/diagnostics_relay.h
 * @brief Request iOS diagnostic information from device.
 * \internal
 *
 * Copyright (c) 2012 Martin Szulecki, All Rights Reserved.
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

#ifndef IDIAGNOSTICS_RELAY_H
#define IDIAGNOSTICS_RELAY_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define DIAGNOSTICS_RELAY_SERVICE_NAME "com.apple.mobile.diagnostics_relay"
/** @name Error Codes */
/*@{*/
#define DIAGNOSTICS_RELAY_E_SUCCESS                0
#define DIAGNOSTICS_RELAY_E_INVALID_ARG           -1
#define DIAGNOSTICS_RELAY_E_PLIST_ERROR           -2
#define DIAGNOSTICS_RELAY_E_MUX_ERROR             -3
#define DIAGNOSTICS_RELAY_E_UNKNOWN_REQUEST       -4

#define DIAGNOSTICS_RELAY_E_UNKNOWN_ERROR       -256
/*@}*/

#define DIAGNOSTICS_RELAY_ACTION_FLAG_WAIT_FOR_DISCONNECT (1 << 1)
#define DIAGNOSTICS_RELAY_ACTION_FLAG_DISPLAY_PASS        (1 << 2)
#define DIAGNOSTICS_RELAY_ACTION_FLAG_DISPLAY_FAIL        (1 << 3)

#define DIAGNOSTICS_RELAY_REQUEST_TYPE_ALL                "All"
#define DIAGNOSTICS_RELAY_REQUEST_TYPE_WIFI               "WiFi"
#define DIAGNOSTICS_RELAY_REQUEST_TYPE_GAS_GAUGE          "GasGauge"
#define DIAGNOSTICS_RELAY_REQUEST_TYPE_NAND               "NAND"

/** Represents an error code. */
typedef int16_t diagnostics_relay_error_t;

typedef struct diagnostics_relay_client_private diagnostics_relay_client_private;
typedef diagnostics_relay_client_private *diagnostics_relay_client_t; /**< The client handle. */

LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_client_new(idevice_t device, lockdownd_service_descriptor_t service, diagnostics_relay_client_t *client);
	
/**
 * Starts a new diagnostics_relay service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     diagnostics_relay_client_t upon successful return. Must be freed using
 *     diagnostics_relay_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return DIAGNOSTICS_RELAY_E_SUCCESS on success, or an DIAGNOSTICS_RELAY_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_client_start_service(idevice_t device, diagnostics_relay_client_t* client, const char* label);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_client_free(diagnostics_relay_client_t client);

LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_goodbye(diagnostics_relay_client_t client);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_sleep(diagnostics_relay_client_t client);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_restart(diagnostics_relay_client_t client, int flags);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_shutdown(diagnostics_relay_client_t client, int flags);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_request_diagnostics(diagnostics_relay_client_t client, const char* type, plist_t* diagnostics);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_query_mobilegestalt(diagnostics_relay_client_t client, plist_t keys, plist_t* result);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_query_ioregistry_entry(diagnostics_relay_client_t client, const char* name, const char* diag_class, plist_t* result);
LIBIMOBILEDEVICE_API diagnostics_relay_error_t diagnostics_relay_query_ioregistry_plane(diagnostics_relay_client_t client, const char* plane, plist_t* result);

#ifdef __cplusplus
}
#endif

#endif
