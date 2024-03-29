/*
 * socket.h
 *
 * Copyright (c) 2012 Martin Szulecki All Rights Reserved.
 * Copyright (c) 2012 Nikias Bassen All Rights Reserved.
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

#ifndef __SOCKET_SOCKET_H
#define __SOCKET_SOCKET_H

#include "libimobiledevice/exports.h"

#include <stdlib.h>
#include <stdint.h>

enum fd_mode {
	FDM_READ,
	FDM_WRITE,
	FDM_EXCEPT
};
typedef enum fd_mode fd_mode;


#ifdef WIN32
#include <winsock2.h>
#define SHUT_RD SD_READ
#define SHUT_WR SD_WRITE
#define SHUT_RDWR SD_BOTH
#else
#include <sys/socket.h>
#endif

#ifndef WIN32
int socket_create_unix(const char *filename);
int socket_connect_unix(const char *filename);
#endif
LIBIMOBILEDEVICE_API int socket_create(uint16_t port);
LIBIMOBILEDEVICE_API int socket_connect(const char *addr, uint16_t port);
LIBIMOBILEDEVICE_API int socket_check_fd(int fd, fd_mode fdm, unsigned int timeout);
LIBIMOBILEDEVICE_API int socket_accept(int fd, uint16_t port);

LIBIMOBILEDEVICE_API int socket_shutdown(int fd, int how);
LIBIMOBILEDEVICE_API int socket_close(int fd);

LIBIMOBILEDEVICE_API int socket_receive(int fd, void *data, size_t size);
LIBIMOBILEDEVICE_API int socket_peek(int fd, void *data, size_t size);
LIBIMOBILEDEVICE_API int socket_receive_timeout(int fd, void *data, size_t size, int flags,unsigned int timeout);
LIBIMOBILEDEVICE_API int socket_send(int fd, void *data, size_t size);
LIBIMOBILEDEVICE_API void socket_set_verbose(int level);

#endif	/* __SOCKET_SOCKET_H */
