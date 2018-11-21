#pragma once
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
# define strdup _strdup
#endif
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/notification_proxy.h>
#include <libimobiledevice/afc.h>
#include "../../libusbmuxd/usbmuxd-proto.h"
#include <plist/plist.h>
#include <string>
#include <dirent.h>
#include <vector>
#include <getopt.h>
//#include "version.h"

#if defined ( WIN32 )
#define __STDC_FORMAT_MACROS
#define __func__ __FUNCTION__
#ifndef S_ISREG
# define S_ISREG(B) ((B)&_S_IFREG)
#endif
#ifndef S_ISDIR
# define S_ISDIR(B) ((B)&_S_IFDIR)
#endif
#endif

#ifndef WIN32
#define Sleep(x) usleep(x*1000)
#endif

#define ITUNES_METADATA_PLIST_FILENAME "iTunesMetadata.plist"
const char PKG_PATH[] = "PublicStaging";
//const char APPARCH_PATH[] = "ApplicationArchives";

using namespace std;
extern int notified;
extern char *last_status;
extern int wait_for_op_complete;
extern int notification_expected;
extern int is_device_connected;
extern int op_completed;
extern int err_occured;
extern char* udid;
extern int err_occured;
extern int err_occured;
extern int installer_port;


void log_ideviceinstaller_va(const char *psz_format, va_list args);
void log_ideviceinstaller(const char *psz_format, ...);
static bool str_is_udid(const char* str);
bool update_usbmuxd_host(int argc, char** argv);
void parse_opts(int argc, char** argv);
void print_usage(int argc, char** argv);
void do_wait_when_needed();
std::string decode(std::string const & s);

#define ON_CMDLINE_ERRROR {print_usage(argc, argv); return 0;}

#ifdef HAVE_LIBIMOBILEDEVICE_1_1
static void notifier(const char *notification, void *unused)
#else
static void notifier(const char *notification)
#endif
{
	printf("\n\n notification received: %s\n", notification);
	notified = 1;
}

class Services
{
private:
	idevice_t m_phone = NULL;
	lockdownd_client_t m_client = NULL;
	instproxy_client_t m_ipc = NULL;
	np_client_t m_np = NULL;
	afc_client_t m_afc = NULL;
	lockdownd_service_descriptor_t m_service = NULL;
	bool initServicesInternal();

public:
	instproxy_client_t getInstproxyClient()
	{
		return m_ipc;
	}
	afc_client_t getAfcClient()
	{
		return m_afc;
	}
	Services(){}
	~Services(){}
	bool initServices();
	bool initAfcService();
	void stop();
};

