// idevice_installer_server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define _GNU_SOURCE 1
#define __USE_GNU 1

#include <iostream> 
#include <unistd.h>

#include "PmasServer.h"
#include "installer_utils.h"
#include "command.h"

#ifdef WIN32
#include "GExceptionUtils.h"
#include "GMiniDump.h"
#endif

//#define DETECT_LEAKS 1
#ifdef DETECT_LEAKS
#include "vld.h"
#endif

#ifndef PRIu64
# define PRIu64 "llu"
#endif
#ifndef PRIi64
# define PRIi64 "ll"
#endif

int installer_port = 0;
int quit_flag = 0;

// PMAS STUFF
//=============

#define CONNECT_CMD "CONNECT"
#define PING_CMD "PING"
#define LIST_CMD "LIST"
#define INSTALL_CMD "INSTALL"
#define UNINSTALL_CMD "UNINSTALL"
#define EXIT_CMD "EXIT"
TCPConnection *pConnection = NULL;

TCPServer *pPmasServer;
class ConnectRequestHandler : public RequestHandler {
	virtual void HandleRequest(vector<string>& Params) {
		log_ideviceinstaller(CONNECT_CMD " received");
	}
};


class PingRequestHandler : public RequestHandler {
	virtual void HandleRequest(vector<string>& Params) {
		log_ideviceinstaller(PING_CMD " received");
		pConnection->Send("PONG");
	}
};

class ExitRequestHandler : public RequestHandler {
	virtual void HandleRequest(vector<string>& Params) {
		log_ideviceinstaller(EXIT_CMD " received");
		pConnection->SendInt(0);
		delete pPmasServer;
		quit_flag = 1;
	}
};

//Services* service;
bool SendResult(string* res)
{
	bool result = false;
	result = pConnection->SendInt(0);
	result = pConnection->SendInt(res->size());
	result = pConnection->Send(*res);
	return result;
}
class ListRequestHandler : public RequestHandler
{
	virtual void HandleRequest(vector<string>& Params)
	{
		if (Params.size() != 2)
		{
			log_ideviceinstaller("ListRequestHandler: Invalid number of parameters (%d expected but %d received)", 2, Params.size());
			return;
		}
		bool res = false;
		string* result = NULL;

		log_ideviceinstaller(LIST_CMD " received");
		Services* service = new Services();
		res = service->initServices();
		ListCommand list(Params);
		if (res)
		{
			result = list.getListFromDevice(service);
			//cout << *result << endl;
		}
		if (!res || result == NULL)
		{
			log_ideviceinstaller("ListRequestHandler: FAILED");
			pConnection->SendInt(1);
		}
		else
		{
			SendResult(result);
		}
			
		service->stop();
		delete service;
	}
};

class InstallRequestHandler : public RequestHandler
{
	virtual void HandleRequest(vector<string>& Params)
	{
		if (Params.size() != 1)
		{
			log_ideviceinstaller("InstallRequestHandler: Invalid number of parameters (%d expected but %d received)", 1, Params.size());
			return;
		}

		log_ideviceinstaller(INSTALL_CMD " received");
		Services* service = new Services();
		InstallCommand install(Params);
		bool res = false;
		res = service->initServices();
		if (res)
			res = service->initAfcService();
		if (res)
			res = install.copyAppOnDevice(service);
		if (res)
			res = install.installAndVerify(service, NULL, NULL);
		/**************************************NP-23112****************************************/
		/*                                  SLEEP     1500                                    */
		/* We can start application only after SpringBoard got message and  represented icon. */
		/* If installd finished to install successfully and we try to start app, it will fail.*/
		/**************************************************************************************/
		if (res)
		{
			log_ideviceinstaller("Installed and verified: holding for 1500 msec to make sure that springboard state has been refreshed");
			Sleep(1500);
		}
		else {
			log_ideviceinstaller("InstallRequestHandler: FAILED");
			pConnection->SendInt(1);
		}

		string bundleID = string(install.bundleIdentifier());
		SendResult(&bundleID);
		log_ideviceinstaller("InstallRequestHandler: result - %s, BundelID - %s", res ? "TRUE" : "FALSE" , install.bundleIdentifier());
		do_wait_when_needed();
		notified = 0;
		service->stop();
		delete service;
	}
};

class UninstallRequestHandler : public RequestHandler
{
	virtual void HandleRequest(vector<string>& Params)
	{
		if (Params.size() != 2)
		{
			log_ideviceinstaller("UnInstallRequestHandler: Invalid number of parameters (%d expected but %d received)", 2, Params.size());
			return;
		}

		log_ideviceinstaller(UNINSTALL_CMD " received with prams: name - %s id - %s", Params[0].c_str(), Params[1].c_str());
		Services* service = new Services();
		UninstallCommand uninstall(Params);
		bool res = false;
		res = service->initServices();
		if (res)
			res = uninstall.uninstall(service);
		int result = res ? 0 : 1;
		pConnection->SendInt(result);
		log_ideviceinstaller("UnInstallRequestHandler: result - %d", result);
		do_wait_when_needed();
		notified = 0;
		service->stop();
		delete service;
	}
};

class InstallerConnectionHandler : public ConnectionHandler
{
public:
	virtual void HandleData(TCPConnection *pConn)
	{
		log_ideviceinstaller("Connection with client established");
		pConnection = pConn;
		if (!pConn->RegisterHandler(CONNECT_CMD, 0, new ConnectRequestHandler))
		{
			log_ideviceinstaller("Failed to register " CONNECT_CMD " handler.");
			quit_flag = true;
		}
		if (!pConn->RegisterHandler(PING_CMD, 0, new PingRequestHandler))
		{
			log_ideviceinstaller("Failed to register " PING_CMD " handler.");
			quit_flag = true;
		}
		if (!pConn->RegisterHandler(LIST_CMD, 2, new ListRequestHandler))
		{
			log_ideviceinstaller("Failed to register " LIST_CMD " handler.");
			quit_flag = true;
		}
		if (!pConn->RegisterHandler(INSTALL_CMD, 1, new InstallRequestHandler))
		{
			log_ideviceinstaller("Failed to register " INSTALL_CMD " handler.");
			quit_flag = true;
		}
		if (!pConn->RegisterHandler(UNINSTALL_CMD, 2, new UninstallRequestHandler))
		{
			log_ideviceinstaller("Failed to register " UNINSTALL_CMD " handler.");
			quit_flag = true;
		}
		if (!pConn->RegisterHandler(EXIT_CMD, 0, new ExitRequestHandler))
		{
			log_ideviceinstaller("Failed to register " EXIT_CMD " handler.");
			quit_flag = true;
		}

		if (!pConn->HandleAll())
		{
			log_ideviceinstaller("Failed to start handler loop");
			quit_flag = true;
		}
	}

};

class WPErrorHandler : public ConnectionErrorHandler
{
public:
	virtual void HandleError(int iErr) {}

	virtual void OnConnectionClosed(void)
	{
		if (!pPmasServer->WaitConnection())
		{
			log_ideviceinstaller("Failed to connect Pmas TCP server.");
		}
	}

	virtual void LogError(const char *psz_format, ...)
	{
		va_list args;
		va_start(args, psz_format);
		log_ideviceinstaller(psz_format, args);
		va_end(args);
	}
};


void run_pmas_server(int pmas_port)
{
	pPmasServer = new TCPServer;
	// start server
	if (pmas_port)
	{
		log_ideviceinstaller("Starting Pmas server on port %d", pmas_port);
		if (!pPmasServer->Start(pmas_port, new InstallerConnectionHandler, new WPErrorHandler))
		{
			log_ideviceinstaller("Failed to start Pmas TCP server.");
		}
		log_ideviceinstaller("Pmas server started successfully!");
	}


	pPmasServer->ResetConnection();
	if (!pPmasServer->WaitConnection())
	{
		log_ideviceinstaller("Failed to connect Pmas TCP server.");
	}
}



// END Pmas server Stuff
// =====================
static void clean_exit(int sig)
{
	fprintf(stderr, "Exiting...\n");
	quit_flag++;
}
#include <signal.h>
int main(int argc, char **argv)
{
#ifdef WIN32
	SetDumpPrefix(L"idevice_installer_server");
	CRASH_GUARD_BLOCK_START
#endif

	parse_opts(argc, argv);

	log_ideviceinstaller("idevice_installer_server: start!\n");
//	print_version();

	signal(SIGINT, clean_exit);
	signal(SIGTERM, clean_exit);
	if (!update_usbmuxd_host(argc, argv))
		ON_CMDLINE_ERRROR

	run_pmas_server(installer_port);
	//service = new Services();
	//service->initServices();

	while (!quit_flag)
	{
		Sleep(100);
	}

	free(udid);
	//	service->stop();
	//delete service;

#ifdef WIN32
	CRASH_GUARD_BLOCK_END
#endif
	return 0;
}
