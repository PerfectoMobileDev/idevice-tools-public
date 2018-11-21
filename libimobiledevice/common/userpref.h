/*
 * userpref.h
 * contains methods to access user specific certificates IDs and more.
 *
 * Copyright (c) 2008 Jonathan Beck All Rights Reserved.
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

#ifndef __USERPREF_H
#define __USERPREF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_OPENSSL
typedef struct {
	unsigned char *data;
	unsigned int size;
} key_data_t;
#else
#include <gnutls/gnutls.h>
typedef gnutls_datum_t key_data_t;
#endif

#include "libimobiledevice/exports.h"

#include <stdint.h>
#include <plist/plist.h>
#ifndef LIBIMOBILEDEVICE_INTERNAL
#ifdef WIN32
#define LIBIMOBILEDEVICE_INTERNAL
#else
#define LIBIMOBILEDEVICE_INTERNAL __attribute__((visibility("hidden")))
#endif
#endif
#define USERPREF_DEVICE_CERTIFICATE_KEY "DeviceCertificate"
#define USERPREF_ESCROW_BAG_KEY "EscrowBag"
#define USERPREF_HOST_CERTIFICATE_KEY "HostCertificate"
#define USERPREF_ROOT_CERTIFICATE_KEY "RootCertificate"
#define USERPREF_HOST_PRIVATE_KEY_KEY "HostPrivateKey"
#define USERPREF_ROOT_PRIVATE_KEY_KEY "RootPrivateKey"
#define USERPREF_HOST_ID_KEY "HostID"
#define USERPREF_SYSTEM_BUID_KEY "SystemBUID"
#define USERPREF_WIFI_MAC_ADDRESS_KEY "WiFiMACAddress"

#define USERPREF_E_SUCCESS             0
#define USERPREF_E_INVALID_ARG        -1
#define USERPREF_E_INVALID_CONF       -2
#define USERPREF_E_SSL_ERROR          -3
#define USERPREF_E_READ_ERROR         -4
#define USERPREF_E_WRITE_ERROR        -5

#define USERPREF_E_UNKNOWN_ERROR    -256

typedef int16_t userpref_error_t;

LIBIMOBILEDEVICE_API const char *userpref_get_config_dir(void);
LIBIMOBILEDEVICE_API int userpref_read_system_buid(char **system_buid);
LIBIMOBILEDEVICE_API userpref_error_t userpref_read_pair_record(const char *udid, plist_t *pair_record);
LIBIMOBILEDEVICE_API userpref_error_t userpref_save_pair_record(const char *udid, plist_t pair_record);
LIBIMOBILEDEVICE_API userpref_error_t userpref_delete_pair_record(const char *udid);

LIBIMOBILEDEVICE_API userpref_error_t pair_record_generate_keys_and_certs(plist_t pair_record, key_data_t public_key);
#ifdef HAVE_OPENSSL
LIBIMOBILEDEVICE_API userpref_error_t pair_record_import_key_with_name(plist_t pair_record, const char* name, key_data_t* key);
LIBIMOBILEDEVICE_API userpref_error_t pair_record_import_crt_with_name(plist_t pair_record, const char* name, key_data_t* cert);
LIBIMOBILEDEVICE_INTERNAL userpref_error_t userpref_get_keys_and_certs(key_data_t* root_privkey, key_data_t* root_crt, key_data_t* host_privkey, key_data_t* host_crt);
#else
LIBIMOBILEDEVICE_INTERNAL userpref_error_t userpref_get_keys_and_certs(gnutls_x509_privkey_t root_privkey, gnutls_x509_crt_t root_crt, gnutls_x509_privkey_t host_privkey, gnutls_x509_crt_t host_crt);
userpref_error_t pair_record_import_key_with_name(plist_t pair_record, const char* name, gnutls_x509_privkey_t key);
userpref_error_t pair_record_import_crt_with_name(plist_t pair_record, const char* name, gnutls_x509_crt_t cert);

#endif
LIBIMOBILEDEVICE_INTERNAL userpref_error_t userpref_set_keys_and_certs(key_data_t * root_key, key_data_t * root_cert, key_data_t * host_key, key_data_t * host_cert);
LIBIMOBILEDEVICE_INTERNAL userpref_error_t userpref_get_certs_as_pem(key_data_t *pem_root_cert, key_data_t *pem_host_cert);
LIBIMOBILEDEVICE_INTERNAL userpref_error_t userpref_set_device_public_key(const char *udid, key_data_t public_key);
LIBIMOBILEDEVICE_API userpref_error_t userpref_remove_device_public_key(const char *udid);
LIBIMOBILEDEVICE_INTERNAL int userpref_has_device_public_key(const char *udid);

LIBIMOBILEDEVICE_API userpref_error_t pair_record_get_host_id(plist_t pair_record, char** host_id);
LIBIMOBILEDEVICE_API userpref_error_t pair_record_set_host_id(plist_t pair_record, const char* host_id);
LIBIMOBILEDEVICE_API userpref_error_t pair_record_get_item_as_key_data(plist_t pair_record, const char* name, key_data_t *value);
LIBIMOBILEDEVICE_API userpref_error_t pair_record_set_item_from_key_data(plist_t pair_record, const char* name, key_data_t *value);

LIBIMOBILEDEVICE_API userpref_error_t userpref_get_paired_udids(char ***list, unsigned int *count);
LIBIMOBILEDEVICE_API void userpref_get_host_id(char **host_id);
LIBIMOBILEDEVICE_API int userpref_has_pair_record(const char *udid);

#endif
