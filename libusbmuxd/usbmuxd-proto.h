/*
	libusbmuxd - client library to talk to usbmuxd

Copyright (C) 2009	Paul Sladen <libiphone@paul.sladen.org>
Copyright (C) 2009	Nikias Bassen <nikias@gmx.li>
Copyright (C) 2009	Hector Martin "marcan" <hector@marcansoft.com>

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

/* Protocol defintion for usbmuxd proxy protocol */
#ifndef __USBMUXD_PROTO_H
#define __USBMUXD_PROTO_H

#include "exports.h"
#include <stdint.h>

#define USBMUXD_PROTOCOL_VERSION 0

////////////////////////////////////////////////////////////
//	USED from tools to setup usbmuxd host ip and port 
////////////////////////////////////////////////////////////

#define DEF_USBMUXD_IP "127.0.0.1"
#define DEF_USBMUXD_SOCKET_PORT 27015 
#define USBMUXD_SOCKET_FILE "/var/run/usbmuxd"

/////////////////////////////////////////////////////////

LIBUSBMUXD_API const char* get_usbmuxd_ipv4(void);

LIBUSBMUXD_API unsigned int get_usbmuxd_port(void);

LIBUSBMUXD_API void usbmuxd_ipv4(const char* pIP);

LIBUSBMUXD_API void usbmuxd_port(unsigned int aPort);

LIBUSBMUXD_API bool validate_ipv4(const char* pbuff);


#ifdef __cplusplus
extern "C" {
#endif


enum usbmuxd_result {
	RESULT_OK = 0,
	RESULT_BADCOMMAND = 1,
	RESULT_BADDEV = 2,
	RESULT_CONNREFUSED = 3,
	// ???
	// ???
	RESULT_BADVERSION = 6,
};

enum usbmuxd_msgtype {
	MESSAGE_RESULT  = 1,
	MESSAGE_CONNECT = 2,
	MESSAGE_LISTEN = 3,
	MESSAGE_DEVICE_ADD = 4,
	MESSAGE_DEVICE_REMOVE = 5,
	//???
	//???
	MESSAGE_PLIST = 8,
};

#pragma pack(push, 1)

struct usbmuxd_header {
	uint32_t length;    // length of message, including header
	uint32_t version;   // protocol version
	uint32_t message;   // message type
	uint32_t tag;       // responses to this query will echo back this tag
};

#pragma pack(pop)

#pragma pack(push, 1)

struct usbmuxd_result_msg {
	struct usbmuxd_header header;
	uint32_t result;
};

#pragma pack(pop)

#pragma pack(push, 1)

struct usbmuxd_connect_request {
	struct usbmuxd_header header;
	uint32_t device_id;
	uint16_t port;   // TCP port number
	uint16_t reserved;   // set to zero
};

#pragma pack(pop)

#pragma pack(push, 1)

struct usbmuxd_listen_request {
	struct usbmuxd_header header;
};

#pragma pack(pop)

#pragma pack(push, 1)

struct usbmuxd_device_record {
	uint32_t device_id;
	uint16_t product_id;
	char serial_number[256];
	uint16_t padding;
	uint32_t location;
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* __USBMUXD_PROTO_H */
