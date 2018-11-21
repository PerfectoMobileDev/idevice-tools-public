/*
 * utils.h
 * Miscellaneous utilities for string manipulation
 *
 * Copyright (c) 2013 Federico Mena Quintero
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

#ifndef __UTILS_H
#define __UTILS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "libimobiledevice/exports.h"
#include <stdio.h>
#include <plist/plist.h>

#define MAC_EPOCH 978307200

LIBIMOBILEDEVICE_API char *string_concat(const char *str, ...);
LIBIMOBILEDEVICE_API char *string_build_path(const char *elem, ...);
LIBIMOBILEDEVICE_API char *string_format_size(uint64_t size);
LIBIMOBILEDEVICE_API char *string_toupper(char *str);
LIBIMOBILEDEVICE_API char *generate_uuid();


#ifndef HAVE_STPCPY
LIBIMOBILEDEVICE_API char *stpcpy(char * s1, const char * s2);
#endif


enum plist_format_t 
{
	PLIST_FORMAT_XML,
	PLIST_FORMAT_BINARY
};

LIBIMOBILEDEVICE_API int plist_read_from_filename(plist_t *plist, const char *filename);
LIBIMOBILEDEVICE_API int plist_write_to_filename(plist_t plist, const char *filename, enum plist_format_t format);
LIBIMOBILEDEVICE_API void plist_print_to_stream(plist_t plist, FILE* stream);
LIBIMOBILEDEVICE_API void buffer_read_from_filename(const char *filename, char **buffer, uint64_t *length);
LIBIMOBILEDEVICE_API void buffer_write_to_filename(const char *filename, const char *buffer, uint64_t length);

#endif
