/*
 * idevicesyslog.c
 * Relay the syslog of a device to stdout
 *
 * Copyright (c) 2009 Martin Szulecki All Rights Reserved.
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
#ifdef WIN32
#include "stdafx.h"
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <unistd.h>

#ifdef WIN32
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#endif

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/syslog_relay.h>
#include "../../libusbmuxd/usbmuxd-proto.h"
#ifdef WIN32
#include "GExceptionUtils.h"
#include "GMiniDump.h"
#else
#include <sys/time.h>
#define _strdup strdup
#endif



static int quit_flag = 0;
void print_usage(int argc, char **argv);
static char* udid = NULL;
static idevice_t device = NULL;
static syslog_relay_client_t syslog = NULL;

#define MAX_NOLOG_TIMEOUT	120000
unsigned long long _dataTime = 0;

///////////////////////////////////////////////////////////////////////////
//	getTickCount
///////////////////////////////////////////////////////////////////////////
unsigned long long getTickCount()
{
#ifdef WIN32
	return (unsigned long long) GetTickCount();
#else
	struct timeval tv;

	if (gettimeofday(&tv, NULL) != 0)
		return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}
///////////////////////////////////////////////////////////////////////////
//	syslog_callback
///////////////////////////////////////////////////////////////////////////
static void syslog_callback(char c, void *user_data)
{
   putchar(c);
   // case where syslog_relay service dies on the device, only \n is received in a loop
   // if nothing or nothing but newlines is received for MAX_NOLOG_TIMEOUT the process will exit
   if (c != '\n')
	   _dataTime = getTickCount();
}

///////////////////////////////////////////////////////////////////////////
//	dataDisconnectionDetected
///////////////////////////////////////////////////////////////////////////
bool dataDisconnectionDetected()
{
	unsigned long long nowTime = getTickCount();

	if ((nowTime - _dataTime) > MAX_NOLOG_TIMEOUT)
		return true;

	return false;
}


static int start_logging()
{
        idevice_error_t ret = idevice_new(&device, udid);
        if (ret != IDEVICE_E_SUCCESS) 
		{
                fprintf(stderr, "Device with udid %s not found!?\n", udid);
                return -1;
        }

        /* start and connect to syslog_relay service */
        syslog_relay_error_t serr = SYSLOG_RELAY_E_UNKNOWN_ERROR;
        serr = syslog_relay_client_start_service(device, &syslog, "idevicesyslog");
        if (serr != SYSLOG_RELAY_E_SUCCESS) 
		{
                fprintf(stderr, "ERROR: Could not start service com.apple.syslog_relay.\n");
                idevice_free(device);
                device = NULL;
                return -1;
        }

        /* start capturing syslog */
        serr = syslog_relay_start_capture(syslog, syslog_callback, NULL);
        if (serr != SYSLOG_RELAY_E_SUCCESS) 
		{
                fprintf(stderr, "ERROR: Unable tot start capturing syslog.\n");
                syslog_relay_client_free(syslog);
                syslog = NULL;
                idevice_free(device);
                device = NULL;
                return -1;
        }

        fprintf(stdout, "[connected]\n");
        fflush(stdout);

        return 0;
}

static void stop_logging()
{
        fflush(stdout);

        if (syslog) 
		{
			syslog_relay_client_free(syslog);
            syslog = NULL;
        }

        if (device) 
		{
            idevice_free(device);
            device = NULL;
        }
}

int strcmpnocase(const char* pstr1, const char* pstr2)
{
	if (!pstr1 || !pstr2)
		return -1;

	if (strlen(pstr1) != strlen(pstr2))
		return -1;

	int nlen = strlen(pstr1);

	for (int i = 0; i < nlen; i++)
	{
		char c1 = tolower(pstr1[i]);

		char c2 = tolower(pstr2[i]);

		if (c1 != c2) return -1;
	}

	return 0;
}

static void device_event_cb(const idevice_event_t* event, void* userdata)
{
        if (event->event == IDEVICE_DEVICE_ADD) 
		{
                if (!syslog) 
				{
                        if (!udid)  udid = _strdup(event->udid);
						if (strcmpnocase(udid, event->udid) == 0)
						{
                                if (start_logging() != 0)  fprintf(stderr, "Could not start logger for udid %s\n", udid);
                        }
                }
        } 
		else if (event->event == IDEVICE_DEVICE_REMOVE) 
		{
                if (syslog && (strcmpnocase(udid, event->udid) == 0)) 
				{
                        stop_logging();
                        fprintf(stdout, "[disconnected]\n");
                }
        }
}

/**
 * signal handler function for cleaning up properly
 */
static void clean_exit(int sig)
{
        fprintf(stderr, "\nExiting...\n");
        quit_flag++;
}

#define ON_CMDLINE_ERRROR {print_usage(argc, argv); return 0;}




int main(int argc, char *argv[])
{
	#ifdef WIN32
	SetDumpPrefix(L"IdeviceSysLog");
	CRASH_GUARD_BLOCK_START
	#endif

        signal(SIGINT, clean_exit);
        signal(SIGTERM, clean_exit);
#ifndef WIN32
        signal(SIGQUIT, clean_exit);
        signal(SIGPIPE, SIG_IGN);
#endif

        /* parse cmdline args */
		int i;
        for (i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) 
				{
                        idevice_set_debug_level(1);
                        continue;
                }
                else if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--udid")) 
				{
                        i++;
                        if (!argv[i] /*|| (strlen(argv[i]) != 40)*/) {
                                print_usage(argc, argv);
                                return 0;
                        }
                        udid = _strdup(argv[i]);
                        continue;
                }
				else if (!strcmp(argv[i], "-I") || !strcmp(argv[i], "--usbmuxd_ip"))
				{
					i++;
					if (!argv[i] || !validate_ipv4(argv[i]))
						ON_CMDLINE_ERRROR
					else
					usbmuxd_ipv4(argv[i]);
					continue;
				}
				else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--usbmuxd_port"))
				{
					i++;
					if (!argv[i])
						ON_CMDLINE_ERRROR
					else
					usbmuxd_port(atol(argv[i]));
					continue;
				}
                else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) 
					ON_CMDLINE_ERRROR
				else 
					ON_CMDLINE_ERRROR
		}

        int num = 0;
        char **devices = NULL;
        idevice_get_device_list(&devices, &num);
        idevice_device_list_free(devices);
        if (num == 0) 
		{
                if (!udid) 
				{
                   fprintf(stderr, "No device found. Plug in a device or pass UDID with -u to wait for device to be available.\n");
                   return -1;
                } 
				else fprintf(stderr, "Waiting for device with UDID %s to become available...\n", udid);
        }

        idevice_event_subscribe(device_event_cb, NULL);

        while (!quit_flag) 
		{
			sleep(10);
			if (dataDisconnectionDetected())
			{
				fprintf(stderr, "Disconnection has been detected (!event).. exiting! \n");
				return -1;
			}
        }
        
		idevice_event_unsubscribe();
        stop_logging();
 
        if (udid)  free(udid);
        
		#ifdef WIN32
		CRASH_GUARD_BLOCK_END
		#endif
        return 0;
}

void print_usage(int argc, char **argv)
{
        char *name = NULL;
        
        name = strrchr(argv[0], '/');
        printf("Usage: %s [OPTIONS]\n", (name ? name + 1: argv[0]));
        printf("Relay syslog of a connected device.\n\n");
        printf("  -d, --debug\t\tenable communication debugging\n");
        printf("  -u, --udid UDID\ttarget specific device by its 40-digit device UDID\n");
		printf("  -I, --usbmuxd_ip\t\tusbmuxd host ip (default 127.0.0.1)\n");
		printf("  -P, --usbmuxd_port\t\tusbmuxd host port (defualt 20715) \n");
		printf("  -h, --help\t\tprints usage information\n");
        printf("\n");
}
