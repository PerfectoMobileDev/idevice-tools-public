#include "stdafx.h"
#include "installer_utils.h"

#include <cstring>
#include <unistd.h>

using namespace std;

#ifndef WIN32
#define vfprintf_s vfprintf
#endif

FILE *log_file = NULL;
//HANDLE ghLogMutex;
int notified = 0;
char *last_status = NULL;
int wait_for_op_complete = 0;
int notification_expected = 1;
int is_device_connected = 0;
int op_completed = 0;
int err_occured = 0;
char* udid;

void log_ideviceinstaller_va(const char *psz_format, va_list args)
{
	struct tm*	Time;
	time_t		nTime;
	FILE*		log = stderr;


	time(&nTime);
	Time = localtime(&nTime);
	fprintf(log, "%02d/%02d/%04d - %02d:%02d:%02d ", Time->tm_mday,
		Time->tm_mon + 1,
		Time->tm_year + 1900,
		Time->tm_hour,
		Time->tm_min, Time->tm_sec);
	vfprintf_s(log, psz_format, args);
	fprintf(log, "\n");
	fflush(log);
}

void log_ideviceinstaller(const char *psz_format, ...)
{
	va_list args;
	va_start(args, psz_format);
	log_ideviceinstaller_va(psz_format, args);
	va_end(args);
}

unsigned char _val(char c)
{
	if ('0' <= c && c <= '9') { return c - '0'; }
	if ('a' <= c && c <= 'f') { return c + 10 - 'a'; }
	if ('A' <= c && c <= 'F') { return c + 10 - 'A'; }
	return c;
}

std::string decode(std::string const & s)
{
	std::string result;
	result.reserve(s.size() / 2);
	for (std::size_t i = 0; i < s.size() / 2; ++i)
	{
		unsigned char n = _val(s[2 * i]) * 16 + _val(s[2 * i + 1]);
		result += n;
	}

	return result;
}
static bool str_is_udid(const char* str)
{
	const char allowed[] = "0123456789abcdefABCDEF-";

	/* handle NULL case */
	if (str == NULL) return false;

	int length = strlen(str);

	/* verify length */
	//if (length != 40) return false;

	/* check for invalid characters */
	while (length--)
	{
		/* invalid character in udid? */
		if (strchr(allowed, str[length]) == NULL) return false;
	}

	return true;
}

bool update_usbmuxd_host(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-I") || !strcmp(argv[i], "--usbmuxd_ip"))
		{
			i++;
			if (!argv[i] || !validate_ipv4(argv[i]))
				return false;
			else
				usbmuxd_ipv4(argv[i]);
			continue;
		}
		else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--usbmuxd_port"))
		{
			i++;
			if (!argv[i])
				return false;
			else
				usbmuxd_port(atol(argv[i]));
			continue;
		}
	}
	return true;
}

void print_usage(int argc, char **argv)
{
	char *name = NULL;
	name = strrchr(argv[0], '/');
	printf("Usage: %s OPTIONS\n", (name ? name + 1 : argv[0]));
	printf("Manage apps on an iDevice.\n\n");
	printf
		("  -U, --udid UDID\tTarget specific device by its 40-digit device UDID.\n"
		"	-I, --usbmuxd_ip\t\tusbmuxd host ip (default 127.0.0.1)\n"
		"	-P, --usbmuxd_port\t\tusbmuxd host port (defualt 20715)\n"
		"  -h, --help\t\tprints usage information\n"
		"  -d, --debug\t\tenable communication debugging\n" "\n"
		"  -p, --port\t\tTCP server port\n"
		"  -v, --version\t\tversion\n");
}

void parse_opts(int argc, char** argv)
{
	static struct option longopts[] = {
		{ "help", 0, NULL, 'h' },
		{ "udid", 1, NULL, 'U' },
		{ "usbmuxd_ip", 1, NULL, 'I' },
		{ "usbmuxd_port", 1, NULL, 'P' },
		{ "port", 1, NULL, 'p' },
		{ "debug", 0, NULL, 'd' },
		{ "version", 0, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	int c;

	while (1) {
		c = getopt_long(argc, argv, "hU:d:I:p:P:v", longopts, (int *)0);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h':
			print_usage(argc, argv);
			exit(0);
		case 'U':
			if (!str_is_udid(optarg))
			{
				printf("%s: invalid UDID specified !\n", argv[0]);
				print_usage(argc, argv);
				exit(2);
			}
			udid = strdup(optarg);
			break;
		case 'I':
			usbmuxd_ipv4(optarg);
			break;
		case 'P':
			usbmuxd_port(atol(optarg));
			break;
		case 'p':
			installer_port = atoi(optarg);
			break;
		case 'd':
			idevice_set_debug_level(1);
			break;
		case 'v':
//			print_version();
			exit(0);
		default:
			print_usage(argc, argv);
			exit(2);
		}
	}

	if (optind <= 1 || (argc - optind > 0)) {
		print_usage(argc, argv);
		exit(2);
	}
}

static void status_cb1(const char *operation, plist_t status, void *unused)
{
	if (status && operation)
	{
		plist_t npercent = plist_dict_get_item(status, "PercentComplete");
		plist_t nstatus = plist_dict_get_item(status, "Status");
		plist_t nerror = plist_dict_get_item(status, "Error");
		int percent = 0;
		char *status_msg = NULL;
		if (npercent)
		{
			uint64_t val = 0;
			plist_get_uint_val(npercent, &val);
			percent = val;
		}
		if (nstatus)
		{
			plist_get_string_val(nstatus, &status_msg);
			if (!strcmp(status_msg, "Complete"))
			{
				op_completed = 1;
			}
		}
		if (!nerror)
		{
			if (last_status && (strcmp(last_status, status_msg)))
			{
				printf("\r");
			}

			if (!npercent)
			{
				printf("%s - %s\n", operation, status_msg);
			}
			else
			{
				printf("%s - %s (%d%%)\n", operation, status_msg, percent);
			}
		}
		else
		{
			char *err_msg = NULL;
			plist_get_string_val(nerror, &err_msg);
			printf("%s - Error occured: %s\n", operation, err_msg);
			free(err_msg);
			err_occured = 1;
		}
		if (last_status)
		{
			free(last_status);
			last_status = NULL;
		}
		if (status_msg)
		{
			last_status = strdup(status_msg);
			free(status_msg);
		}
	}
	else
	{
		printf("%s: called with invalid data!\n", __func__);
	}
}
#ifdef HAVE_LIBIMOBILEDEVICE_1_1_5
void do_wait_when_needed()
{
	int i = 0;
#ifndef WIN32
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000000;
#else
	unsigned long sleep_duration = 500;
#endif
	/* wait for operation to complete */
	while (wait_for_op_complete && !op_completed && !err_occured
		&& !notified && (i < 60)) {
#ifndef WIN32
		nanosleep(&ts, NULL);
#else
		Sleep(sleep_duration);
#endif
		i++;
	}

	/* wait some time if a notification is expected */
	while (notification_expected && !notified && !err_occured && (i < 10)) {
#ifndef WIN32
		nanosleep(&ts, NULL);
#else
		Sleep(sleep_duration);
#endif
		i++;
	}
}
#else
static void idevice_wait_for_operation_to_complete()
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000000;
	is_device_connected = 1;

	/* subscribe to make sure to exit on device removal */
	idevice_event_subscribe(idevice_event_callback, NULL);

	/* wait for operation to complete */
	while (wait_for_op_complete && !op_completed && !err_occured
		&& !notified && is_device_connected) {
		nanosleep(&ts, NULL);
	}

	/* wait some time if a notification is expected */
	while (notification_expected && !notified && !err_occured && is_device_connected) {
		nanosleep(&ts, NULL);
	}

	idevice_event_unsubscribe();
}
#endif 


bool Services::initServices()
{
	bool res = initServicesInternal();
	int i = 0;
	while (!res && i < 5)
	{
		log_ideviceinstaller("ERROR: Services Initialization Failed - RETRY!");
		stop();
		Sleep(1000);
		res = initServicesInternal();
		i++;
	}
	return res;
}

bool Services::initServicesInternal()
{
	if (IDEVICE_E_SUCCESS != idevice_new(&m_phone, udid))
	{
		log_ideviceinstaller("No iPhone found, is it plugged in?\n");
		return false;
	}

	if (LOCKDOWN_E_SUCCESS != lockdownd_client_new_with_handshake(m_phone, &m_client, "ideviceinstaller"))
	{
		log_ideviceinstaller("Could not connect to lockdownd. Exiting.\n");
		return false;
	}

	log_ideviceinstaller("ideviceinstaller: client initialized");

	if ((lockdownd_start_service
		(m_client, "com.apple.mobile.notification_proxy",
		&m_service) != LOCKDOWN_E_SUCCESS) || !m_service) {
		log_ideviceinstaller("Could not start com.apple.mobile.notification_proxy!\n");
		return false;
	}

	log_ideviceinstaller("ideviceinstaller: service initialized");

	np_error_t nperr = np_client_new(m_phone, m_service, &m_np);

	if (m_service)
	{
		lockdownd_service_descriptor_free(m_service);
	}
	m_service = NULL;

	if (nperr != NP_E_SUCCESS)
	{
		log_ideviceinstaller("Could not connect to notification_proxy!\n");
		return false;
	}
	np_set_notify_callback(m_np, notifier, NULL);
	const char *noties[3] = { NP_APP_INSTALLED, NP_APP_UNINSTALLED, NULL };
	np_observe_notifications(m_np, noties);

	if (m_service)
	{
		lockdownd_service_descriptor_free(m_service);
	}
	m_service = NULL;

	if ((lockdownd_start_service(m_client, "com.apple.mobile.installation_proxy", &m_service) != LOCKDOWN_E_SUCCESS) || !m_service)
	{
		log_ideviceinstaller("Could not start com.apple.mobile.installation_proxy!\n");
		return false;
	}
	log_ideviceinstaller("ideviceinstaller: after starting installation_proxy");
	instproxy_error_t err = instproxy_client_new(m_phone, m_service, &m_ipc);

	if (m_service)
	{
		lockdownd_service_descriptor_free(m_service);
	}
	m_service = NULL;

	if (err != INSTPROXY_E_SUCCESS)
	{
		fprintf(stderr, "Could not connect to installation_proxy!\n");
		return false;
	}
	log_ideviceinstaller("ideviceinstaller: after connect to installation_proxy");
	setbuf(stdout, NULL);

	if (last_status)
	{
		free(last_status);
		last_status = NULL;
	}
	notification_expected = 0;
	return true;
}

bool Services::initAfcService()
{
	if (m_service)
	{
		lockdownd_service_descriptor_free(m_service);
	}
	m_service = NULL;

	if ((lockdownd_start_service(m_client, "com.apple.afc", &m_service) != LOCKDOWN_E_SUCCESS) || !m_service)
	{
		log_ideviceinstaller("Could not start com.apple.afc!\n");
		return false;
	}

	lockdownd_client_free(m_client);
	m_client = NULL;

	if (afc_client_new(m_phone, m_service, &m_afc) != AFC_E_SUCCESS)
	{
		log_ideviceinstaller("Could not connect to AFC!\n");
		return false;
	}

	return true;
}

void Services::stop()
{
	if (m_np)
	{
		np_client_free(m_np);
	}
	if (m_ipc)
	{
		instproxy_client_free(m_ipc);
	}
	if (m_afc)
	{
		afc_client_free(m_afc);
	}
	if (m_client)
	{
		lockdownd_client_free(m_client);
	}
	if (m_service)
	{
		lockdownd_service_descriptor_free(m_service);
	}
	idevice_free(m_phone);

}
