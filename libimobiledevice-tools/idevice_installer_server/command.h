#pragma once
#include <vector>
#include "installer_utils.h"
#include <stdio.h>
#ifdef WIN32
#include <asprintf.h>
#endif
#include <libgen.h>
#include <zip.h>

using namespace std;

class ListCommand
{
private:
	plist_t m_client_opts;
	string* m_result;
	plist_t m_apps;
	string m_type;
	string m_output_mode;

public:
	ListCommand(vector<string>& Params);
	~ListCommand()
	{
		if (m_result)
			delete m_result;
		if (m_apps)
			plist_free(m_apps);
		if (m_client_opts)
			instproxy_client_options_free(m_client_opts);
	}
	string* getListFromDevice(Services* service);

private:
	void addClientOpts();
	string* collectAppsInfo();
};

class InstallCommand
{

private:
	char* m_pkgname;
	char* m_bundleidentifier;
	string m_appid;
	plist_t m_client_opts;

public:
	InstallCommand(vector<string>& Params);
	~InstallCommand()
	{
		if (m_client_opts)
			plist_free(m_client_opts);
		if (m_bundleidentifier)
			free(m_bundleidentifier);
		if (m_pkgname)
			free(m_pkgname);
	}
	bool copyAppOnDevice(Services* service);
	bool  installAndVerify(Services* service, instproxy_status_cb_t status_cb, void *user_data);
	char* bundleIdentifier();

private:
	void afc_upload_dir(afc_client_t afc, const char* path, const char* afcpath);
	bool afc_upload_file(afc_client_t afc, const char* filename, const char* dstfn);
	bool zip_get_contents(struct zip *zf, const char *filename, int locate_flags, char **buffer, uint32_t *len);
	bool zip_get_app_directory(struct zip* zf, char** path);
	bool uploadDeveloperApp(afc_client_t afc);
	bool uploadIpa(afc_client_t afc);
	bool verifyAfterInstall(Services* service);
};

class UninstallCommand
{

private:
	string m_appid;
	string m_app_name;

public:
	UninstallCommand(vector<string>& Params)
	{
		m_appid = Params[0];
		m_app_name = Params[1];
	}
	~UninstallCommand(){}
	bool  uninstall(Services* service);

private:
	string getAppIdFromAppName(Services* service, string app_id);
};
