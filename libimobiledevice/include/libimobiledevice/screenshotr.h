/**
 * @file libimobiledevice/screenshotr.h
 * @brief Retrieve a screenshot from device.
 * @note Requires a mounted developer image.
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

#ifndef ISCREENSHOTR_H
#define ISCREENSHOTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define SCREENSHOTR_SERVICE_NAME "com.apple.mobile.screenshotr"
/** @name Error Codes */
/*@{*/
#define SCREENSHOTR_E_SUCCESS                0
#define SCREENSHOTR_E_INVALID_ARG           -1
#define SCREENSHOTR_E_PLIST_ERROR           -2
#define SCREENSHOTR_E_MUX_ERROR             -3
#define SCREENSHOTR_E_BAD_VERSION           -4

#define SCREENSHOTR_E_UNKNOWN_ERROR       -256
/*@}*/

/** Represents an error code. */
typedef int16_t screenshotr_error_t;

typedef struct screenshotr_client_private screenshotr_client_private;
typedef screenshotr_client_private *screenshotr_client_t; /**< The client handle. */

LIBIMOBILEDEVICE_API screenshotr_error_t screenshotr_client_new(idevice_t device, lockdownd_service_descriptor_t service, screenshotr_client_t * client);

/**
 * Starts a new screenshotr service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     screenshotr_client_t upon successful return. Must be freed using
 *     screenshotr_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return SCREENSHOTR_E_SUCCESS on success, or an SCREENSHOTR_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API screenshotr_error_t screenshotr_client_start_service(idevice_t device, screenshotr_client_t* client, const char* label);
LIBIMOBILEDEVICE_API screenshotr_error_t screenshotr_client_free(screenshotr_client_t client);
LIBIMOBILEDEVICE_API screenshotr_error_t screenshotr_take_screenshot(screenshotr_client_t client, char **imgdata, uint64_t *imgsize);

#ifdef __cplusplus
}
#endif

#endif
