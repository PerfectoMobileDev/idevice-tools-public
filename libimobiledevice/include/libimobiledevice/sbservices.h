/**
 * @file libimobiledevice/sbservices.h
 * @brief Manage SpringBoard icons and retrieve icon images.
 * \internal
 *
 * Copyright (c) 2009 Nikias Bassen All Rights Reserved.
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

#ifndef ISB_SERVICES_H
#define ISB_SERVICES_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define SBSERVICES_SERVICE_NAME "com.apple.springboardservices"
/** @name Error Codes */
/*@{*/
#define SBSERVICES_E_SUCCESS                0
#define SBSERVICES_E_INVALID_ARG           -1
#define SBSERVICES_E_PLIST_ERROR           -2
#define SBSERVICES_E_CONN_FAILED           -3

#define SBSERVICES_E_UNKNOWN_ERROR       -256
/*@}*/

/** @name Orientation of the user interface on the device */
/*@{*/
typedef enum {
  SBSERVICES_INTERFACE_ORIENTATION_UNKNOWN                = 0,
  SBSERVICES_INTERFACE_ORIENTATION_PORTRAIT               = 1,
  SBSERVICES_INTERFACE_ORIENTATION_PORTRAIT_UPSIDE_DOWN   = 2,
  SBSERVICES_INTERFACE_ORIENTATION_LANDSCAPE_RIGHT        = 3,
  SBSERVICES_INTERFACE_ORIENTATION_LANDSCAPE_LEFT         = 4
} sbservices_interface_orientation_t;
/*@}*/

/** Represents an error code. */
typedef int16_t sbservices_error_t;

typedef struct sbservices_client_private sbservices_client_private;
typedef sbservices_client_private *sbservices_client_t; /**< The client handle. */

/* Interface */
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_client_new(idevice_t device, lockdownd_service_descriptor_t service, sbservices_client_t *client);

/**
 * Starts a new sbservices service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     sbservices_client_t upon successful return. Must be freed using
 *     sbservices_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return SBSERVICES_E_SUCCESS on success, or an SBSERVICES_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_client_start_service(idevice_t device, sbservices_client_t* client, const char* label);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_client_free(sbservices_client_t client);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_get_icon_state(sbservices_client_t client, plist_t *state, const char *format_version);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_set_icon_state(sbservices_client_t client, plist_t newstate);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_get_icon_pngdata(sbservices_client_t client, const char *bundleId, char **pngdata, uint64_t *pngsize);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_get_interface_orientation(sbservices_client_t client, sbservices_interface_orientation_t* interface_orientation);
LIBIMOBILEDEVICE_API sbservices_error_t sbservices_get_home_screen_wallpaper_pngdata(sbservices_client_t client, char **pngdata, uint64_t *pngsize);

#ifdef __cplusplus
}
#endif

#endif
