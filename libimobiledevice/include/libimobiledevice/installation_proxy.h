/**
 * @file libimobiledevice/installation_proxy.h
 * @brief Manage applications on a device.
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

#ifndef IINSTALLATION_PROXY_H
#define IINSTALLATION_PROXY_H

#include "exports.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#define INSTPROXY_SERVICE_NAME "com.apple.mobile.installation_proxy"

#define INSTPROXY_E_SUCCESS	0
#define INSTPROXY_E_INVALID_ARG	-1
#define INSTPROXY_E_PLIST_ERROR	-2
#define INSTPROXY_E_CONN_FAILED	-3
#define INSTPROXY_E_OP_IN_PROGRESS	-4
#define INSTPROXY_E_OP_FAILED -5
#define INSTPROXY_E_RECEIVE_TIMEOUT	-6
/* native */
#define INSTPROXY_E_ALREADY_ARCHIVED -7
#define INSTPROXY_E_API_INTERNAL_ERROR -8
#define INSTPROXY_E_APPLICATION_ALREADY_INSTALLED -9
#define INSTPROXY_E_APPLICATION_MOVE_FAILED -10
#define INSTPROXY_E_APPLICATION_SINF_CAPTURE_FAILED -11
#define INSTPROXY_E_APPLICATION_SANDBOX_FAILED -12,
#define INSTPROXY_E_APPLICATION_VERIFICATION_FAILED -13
#define INSTPROXY_E_ARCHIVE_DESTRUCTION_FAILED -14
#define INSTPROXY_E_BUNDLE_VERIFICATION_FAILED -15
#define INSTPROXY_E_CARRIER_BUNDLE_COPY_FAILED -16
#define INSTPROXY_E_CARRIER_BUNDLE_DIRECTORY_CREATION_FAILED -17
#define INSTPROXY_E_CARRIER_BUNDLE_MISSING_SUPPORTED_SIMS -18
#define INSTPROXY_E_COMM_CENTER_NOTIFICATION_FAILED -19
#define INSTPROXY_E_CONTAINER_CREATION_FAILED -20
#define INSTPROXY_E_CONTAINER_P0WN_FAILED -21
#define INSTPROXY_E_CONTAINER_REMOVAL_FAILED -22
#define INSTPROXY_E_EMBEDDED_PROFILE_INSTALL_FAILED -23
#define INSTPROXY_E_EXECUTABLE_TWIDDLE_FAILED -24
#define INSTPROXY_E_EXISTENCE_CHECK_FAILED -25
#define INSTPROXY_E_INSTALL_MAP_UPDATE_FAILED -26
#define INSTPROXY_E_MANIFEST_CAPTURE_FAILED -27
#define INSTPROXY_E_MAP_GENERATION_FAILED -28
#define INSTPROXY_E_MISSING_BUNDLE_EXECUTABLE -29
#define INSTPROXY_E_MISSING_BUNDLE_IDENTIFIER -30
#define INSTPROXY_E_MISSING_BUNDLE_PATH -31
#define INSTPROXY_E_MISSING_CONTAINER -32
#define INSTPROXY_E_NOTIFICATION_FAILED -33
#define INSTPROXY_E_PACKAGE_EXTRACTION_FAILED -34
#define INSTPROXY_E_PACKAGE_INSPECTION_FAILED -35
#define INSTPROXY_E_PACKAGE_MOVE_FAILED -36
#define INSTPROXY_E_PATH_CONVERSION_FAILED -37
#define INSTPROXY_E_RESTORE_CONTAINER_FAILED -38
#define INSTPROXY_E_SEATBELT_PROFILE_REMOVAL_FAILED -39
#define INSTPROXY_E_STAGE_CREATION_FAILED -40
#define INSTPROXY_E_SYMLINK_FAILED -41
#define INSTPROXY_E_UNKNOWN_COMMAND -42
#define INSTPROXY_E_ITUNES_ARTWORK_CAPTURE_FAILED -43
#define INSTPROXY_E_ITUNES_METADATA_CAPTURE_FAILED -44
#define INSTPROXY_E_DEVICE_OS_VERSION_TOO_LOW -45
#define INSTPROXY_E_DEVICE_FAMILY_NOT_SUPPORTED -46
#define INSTPROXY_E_PACKAGE_PATCH_FAILED -47
#define INSTPROXY_E_INCORRECT_ARCHITECTURE -48
#define INSTPROXY_E_PLUGIN_COPY_FAILED -49
#define INSTPROXY_E_BREADCRUMB_FAILED -50
#define INSTPROXY_E_BREADCRUMB_UNLOCK_FAILED -51
#define INSTPROXY_E_GEOJSON_CAPTURE_FAILED -52
#define INSTPROXY_E_NEWSSTAND_ARTWORK_CAPTURE_FAILED -53
#define INSTPROXY_E_MISSING_COMMAND -54
#define INSTPROXY_E_NOT_ENTITLED -55
#define INSTPROXY_E_MISSING_PACKAGE_PATH -56
#define INSTPROXY_E_MISSING_CONTAINER_PATH -57
#define INSTPROXY_E_MISSING_APPLICATION_IDENTIFIER -58
#define INSTPROXY_E_MISSING_ATTRIBUTE_VALUE -59
#define INSTPROXY_E_LOOKUP_FAILED -60
#define INSTPROXY_E_DICT_CREATION_FAILED -61
#define INSTPROXY_E_INSTALL_PROHIBITED -62
#define INSTPROXY_E_UNINSTALL_PROHIBITED -63
#define INSTPROXY_E_MISSING_BUNDLE_VERSION -64

#define INSTPROXY_E_UNKNOWN_ERROR -256

/** Represents an error code. */
typedef int16_t instproxy_error_t;

typedef struct instproxy_client_private instproxy_client_private;
typedef instproxy_client_private *instproxy_client_t; /**< The client handle. */

/** Reports the status of the given operation */
typedef void (*instproxy_status_cb_t) (const char *operation, plist_t status, void *user_data);

/* Interface */
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_client_new(idevice_t device, lockdownd_service_descriptor_t service, instproxy_client_t *client);

/**
 * Starts a new installation_proxy service on the specified device and connects to it.
 *
 * @param device The device to connect to.
 * @param client Pointer that will point to a newly allocated
 *     instproxy_client_t upon successful return. Must be freed using
 *     instproxy_client_free() after use.
 * @param label The label to use for communication. Usually the program name.
 *  Pass NULL to disable sending the label in requests to lockdownd.
 *
 * @return INSTPROXY_E_SUCCESS on success, or an INSTPROXY_E_* error
 *     code otherwise.
 */
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_client_start_service(idevice_t device, instproxy_client_t * client, const char* label);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_client_free(instproxy_client_t client);

LIBIMOBILEDEVICE_API instproxy_error_t instproxy_browse(instproxy_client_t client, plist_t client_options, plist_t *result);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_install(instproxy_client_t client, const char *pkg_path, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_upgrade(instproxy_client_t client, const char *pkg_path, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_uninstall(instproxy_client_t client, const char *appid, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);

LIBIMOBILEDEVICE_API instproxy_error_t instproxy_lookup_archives(instproxy_client_t client, plist_t client_options, plist_t *result);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_archive(instproxy_client_t client, const char *appid, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_restore(instproxy_client_t client, const char *appid, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_remove_archive(instproxy_client_t client, const char *appid, plist_t client_options, instproxy_status_cb_t status_cb, void *user_data);

LIBIMOBILEDEVICE_API plist_t instproxy_client_options_new();
LIBIMOBILEDEVICE_API void instproxy_client_options_add(plist_t client_options, ...);
LIBIMOBILEDEVICE_API void instproxy_client_options_free(plist_t client_options);
LIBIMOBILEDEVICE_API void instproxy_client_options_set_return_attributes(plist_t client_options, ...);

/**
 * Query the device for the path of an application.
 *
 * @param client The connected installation proxy client.
 * @param appid ApplicationIdentifier of app to retrieve the path for.
 * @param path Pointer to store the device path for the application
 *        which is set to NULL if it could not be determined.
 *
 * @return INSTPROXY_E_SUCCESS on success, INSTPROXY_E_OP_FAILED if
 *         the path could not be determined or an INSTPROXY_E_* error
 *         value if an error occured.
 *
 * @note This implementation browses all applications and matches the
 *       right entry by the application identifier.
 */
LIBIMOBILEDEVICE_API instproxy_error_t instproxy_client_get_path_for_bundle_identifier(instproxy_client_t client, const char* bundle_id, char** path);

#ifdef __cplusplus
}
#endif

#endif
