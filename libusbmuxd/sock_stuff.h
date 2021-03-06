/*
	libusbmuxd - client library to talk to usbmuxd

Copyright (C) 2009	Nikias Bassen <nikias@gmx.li>
Copyright (C) 2009	Paul Sladen <libiphone@paul.sladen.org>
Copyright (C) 2009	Martin Szulecki <opensuse@sukimashita.com>

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef __SOCK_STUFF_H
#define __SOCK_STUFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "exports.h"
#include <stdint.h>

enum fd_mode {
	FDM_READ,
	FDM_WRITE,
	FDM_EXCEPT
};
typedef enum fd_mode fd_mode;

#ifdef WIN32
#include <winsock2.h>

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH
#endif

#ifndef WIN32
int create_unix_socket(const char *filename);
int connect_unix_socket(const char *filename);
#endif

LIBUSBMUXD_API int create_socket(uint16_t port);
LIBUSBMUXD_API int connect_socket(const char *addr, uint16_t port);
LIBUSBMUXD_API int check_fd(int fd, fd_mode fdm, unsigned int timeout);
LIBUSBMUXD_API int shutdown_socket(int fd, int how);
LIBUSBMUXD_API int close_socket(int fd);
LIBUSBMUXD_API int recv_buf(int fd, void *data, size_t size);
LIBUSBMUXD_API int peek_buf(int fd, void *data, size_t size);
LIBUSBMUXD_API int recv_buf_timeout(int fd, void *data, size_t size, int flags,unsigned int timeout);
LIBUSBMUXD_API int send_buf(int fd, void *data, size_t size);
LIBUSBMUXD_API void sock_stuff_set_verbose(int level);

#ifdef __cplusplus
}
#endif

#endif							/* __SOCK_STUFF_H */
