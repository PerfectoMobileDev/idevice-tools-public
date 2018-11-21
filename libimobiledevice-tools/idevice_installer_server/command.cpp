#include "stdafx.h"
#include "command.h"
#include <cstring>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <errno.h>
#include <limits.h>

using namespace std;

#ifndef WIN32
#define _strdup strdup
#endif

ListCommand::ListCommand(vector<string>& Params)
{
	m_client_opts = NULL;
	m_result = NULL;
	m_apps = NULL;
	m_type = Params[0].c_str();
	m_output_mode = Params[1].c_str();
}
void ListCommand::addClientOpts()
{
	if (m_type == "user_list")
	{
		m_client_opts = instproxy_client_options_new();
		instproxy_client_options_add(m_client_opts, "ApplicationType", "User", NULL);
	}
		
	else if (m_type == "system_list")
	{
		m_client_opts = instproxy_client_options_new();
		instproxy_client_options_add(m_client_opts, "ApplicationType", "System", NULL);
	}
}

string* ListCommand::collectAppsInfo()
{
	std::ostringstream oss;
	uint32_t i = 0;
	for (i = 0; i < plist_array_get_size(m_apps); i++)
	{
		plist_t app = plist_array_get_item(m_apps, i);
		if (app == NULL)
		{
			log_ideviceinstaller("ERROR: APP is NULL!\n");
			continue;
		}
		plist_t p_appid = plist_dict_get_item(app, "CFBundleIdentifier");
		char *s_appid = NULL;
		char *s_dispName = NULL;
		char *s_version = NULL;
		plist_t dispName = plist_dict_get_item(app, "CFBundleDisplayName");
		plist_t version = plist_dict_get_item(app, "CFBundleVersion");

		if (p_appid)
		{
			plist_get_string_val(p_appid, &s_appid);
		}
		if (!s_appid)
		{
			log_ideviceinstaller("ERROR: Failed to get APPID!\n");
			break;
		}

		if (dispName)
		{
			plist_get_string_val(dispName, &s_dispName);
		}
		if (version)
		{
			plist_get_string_val(version, &s_version);
		}

		if (!s_dispName)
		{
			s_dispName = strdup(s_appid);
		}
		if (s_version)
		{
			oss << s_appid << " - " << s_dispName << " " << s_version << "\n";
			free(s_version);
		}
		else
		{
			oss << s_appid << " - " << s_dispName << "\n";
		}
		free(s_dispName);
		free(s_appid);
	}
	return new string(oss.str());
}

string* ListCommand::getListFromDevice(Services* service)
{
	addClientOpts();
	instproxy_error_t err;
	instproxy_client_t ipc = service->getInstproxyClient();
	err = instproxy_browse(ipc, m_client_opts, &m_apps);
	log_ideviceinstaller("ideviceinstaller: got apps from device");
	if (err != INSTPROXY_E_SUCCESS) {
		log_ideviceinstaller("ERROR: instproxy_browse returned %d\n", err);
		return NULL;
	}
	if (!m_apps || (plist_get_node_type(m_apps) != PLIST_ARRAY)) {
		log_ideviceinstaller("ERROR: instproxy_browse returned an invalid plist!\n");
		return NULL;
	}
	if (m_output_mode == "xml")
	{
		char* xml = NULL;
		uint32_t len = 0;
		plist_to_xml(m_apps, &xml, &len);
		if (!xml)
		{
			return NULL;
		}
		m_result = new string(xml);
		free(xml);
		return m_result;
	}

	log_ideviceinstaller("Total: %d apps\n", plist_array_get_size(m_apps));

	m_result = collectAppsInfo();

	return m_result;
}

InstallCommand::InstallCommand(vector<string>& Params)
{
	m_pkgname = NULL;
	m_bundleidentifier = NULL;
	m_appid = decode(Params[0]);
	m_client_opts = instproxy_client_options_new();

}
bool InstallCommand::afc_upload_file(afc_client_t afc, const char* filename, const char* dstfn)
{
	FILE *f = NULL;
	uint64_t af = 0;
	char buf[8192] = "\0";

	f = fopen(filename, "rb");
	if (!f)
	{
		log_ideviceinstaller("fopen: %s: %s\n", m_appid.c_str(), strerror(errno));
		return false;
	}
	afc_error_t res = AFC_E_UNKNOWN_ERROR;
	res = afc_file_open(afc, dstfn, AFC_FOPEN_WRONLY, &af);
	if ((res != AFC_E_SUCCESS) || !af)
	{
		fclose(f);
		log_ideviceinstaller("afc_file_open on '%s' failed!  ERROR number %" PRId16 "\n", dstfn, res);
		return false;
	}

	size_t amount = 0;
	do {
		amount = fread(buf, 1, sizeof(buf), f);
		if (amount > 0)
		{
			uint32_t written, total = 0;
			while (total < amount)
			{
				written = 0;
				if (afc_file_write(afc, af, buf, amount, &written) != AFC_E_SUCCESS)
				{
					log_ideviceinstaller("AFC Write error!\n");
					break;
				}
				total += written;
			}
			if (total != amount)
			{
				log_ideviceinstaller("Error: wrote only %d of %zu\n", total, amount);
				afc_file_close(afc, af);
				fclose(f);
				return false;
			}
		}
	} while (amount > 0);

	afc_file_close(afc, af);
	fclose(f);

	return true;
}
void InstallCommand::afc_upload_dir(afc_client_t afc, const char* path, const char* afcpath)
{
	afc_make_directory(afc, afcpath);

	DIR *dir = opendir(path);
	if (dir) {
		struct dirent* ep;
		while ((ep = readdir(dir)))
		{
			if ((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0))
			{
				continue;
			}
			char *fpath = (char*)malloc(strlen(path) + 1 + strlen(ep->d_name) + 1);
			char *apath = (char*)malloc(strlen(afcpath) + 1 + strlen(ep->d_name) + 1);

			struct stat st;

			strcpy(fpath, path);
			strcat(fpath, "/");
			strcat(fpath, ep->d_name);

			strcpy(apath, afcpath);
			strcat(apath, "/");
			strcat(apath, ep->d_name);

#ifdef HAVE_LSTAT
			if ((lstat(fpath, &st) == 0) && S_ISLNK(st.st_mode))
			{
				char *target = (char *)malloc(st.st_size + 1);
				if (readlink(fpath, target, st.st_size + 1) < 0)
				{
					log_ideviceinstaller("ERROR: readlink: %s (%d)\n", strerror(errno), errno);
				}
				else
				{
					target[st.st_size] = '\0';
					afc_make_link(afc, AFC_SYMLINK, target, fpath);
				}
				free(target);
			}
			else
#endif
			if ((stat(fpath, &st) == 0) && S_ISDIR(st.st_mode))
			{
				afc_upload_dir(afc, fpath, apath);
			}
			else {
				afc_upload_file(afc, fpath, apath);
			}
			free(fpath);
			free(apath);
		}
		closedir(dir);
	}
}

bool InstallCommand::zip_get_contents(struct zip *zf, const char *filename, int locate_flags, char **buffer, uint32_t *len)
{
	struct zip_stat zs;
	struct zip_file *zfile;
	int zindex = zip_name_locate(zf, filename, locate_flags);

	*buffer = NULL;
	*len = 0;

	if (zindex < 0)
	{
		return false;
	}

	zip_stat_init(&zs);

	if (zip_stat_index(zf, zindex, 0, &zs) != 0)
	{
		log_ideviceinstaller("ERROR: zip_stat_index '%s' failed!\n", filename);
		return false;
	}

	if (zs.size > 10485760)
	{
		log_ideviceinstaller("ERROR: file '%s' is too large!\n", filename);
		return false;
	}

	zfile = zip_fopen_index(zf, zindex, 0);
	if (!zfile)
	{
		log_ideviceinstaller("ERROR: zip_fopen '%s' failed!\n", filename);
		return false;
	}

	*buffer = (char*)malloc(zs.size);
	if (zs.size > LLONG_MAX || zip_fread(zfile, *buffer, zs.size) != (zip_int64_t)zs.size)
	{
		log_ideviceinstaller("ERROR: zip_fread %" PRIu64 " bytes from '%s'\n", (uint64_t)zs.size, filename);
		free(*buffer);
		*buffer = NULL;
		zip_fclose(zfile);
		return false;
	}
	*len = zs.size;
	zip_fclose(zfile);
	return true;
}

bool InstallCommand::zip_get_app_directory(struct zip* zf, char** path)
{
	int i = 0;
	int c = zip_get_num_files(zf);
	int len = 0;
	const char* name = NULL;
	bool res = false;
	/* look through all filenames in the archive */
	do {
		/* get filename at current index */
		name = zip_get_name(zf, i++, 0);
		if (name != NULL) 
		{
			/* check if we have a "Payload/.../" name */
			len = strlen(name);
			if (!strncmp(name, "Payload/", 8) && strstr(name, ".app") && (len > 8)) 
			{
				/* locate the second directory delimiter */
				const char* p = name + 8;
				do {
					if (*p == '/') {
						break;
					}
				} while (*p++ != NULL);

				/* try next entry if not found */
				if (*(p - 1) == NULL)
					continue;

				len = p - name + 1;

				if (*path != NULL) {
					free(*path);
					*path = NULL;
				}

				/* allocate and copy filename */
				*path = (char*)malloc(len + 1);
				strncpy(*path, name, len);

				/* add terminating null character */
				char* t = *path + len;
				*t = '\0';
				res = true;
				break;
			}
		}
	} while (i < c);

	/* check if the path actually exists */
	/*int zindex = zip_name_locate(zf, *path, 0);
	if (zindex < 0) {
		return false;
	}*/

	return res;
}

bool InstallCommand::uploadDeveloperApp(afc_client_t afc)
{
	/* upload developer app directory */
	instproxy_client_options_add(m_client_opts, "PackageType", "Developer", NULL);

	char appId[512];
	strcpy(appId,m_appid.c_str());
	if (asprintf(&m_pkgname, "%s/%s", PKG_PATH, basename(appId)) < 0)
	{
		log_ideviceinstaller("ERROR: Out of memory allocating pkgname!?\n");
		return false;
	}

	log_ideviceinstaller("Uploading %s package contents... ", basename(appId));

	afc_upload_dir(afc, m_appid.c_str(), m_pkgname);

	m_bundleidentifier = (char*)malloc(sizeof("Success") + 1);
	strcpy(m_bundleidentifier, "Success");

	log_ideviceinstaller("DONE.\n");
	return true;
}

std::wstring s2ws(const std::string& s)
{
//	int len;
//	int slength = (int)s.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//	return r;

	wstring ws = wstring(s.begin(), s.end());
	return wstring(ws);
}

//convert char* to wstring
//std::wstring c2ws(const char* s){
//	std::wstring stemp = s2ws(s);
//	LPCWSTR r = stemp.c_str();
//	std::wstring unzipLPCWSTR(r);
//	return &unzipLPCWSTR[0];
//}
//
std::string replaceChar(string str, char ch1, char ch2) {
	for (int i = 0; i < str.length(); ++i) {
		if (str[i] == ch1)
			str[i] = ch2;
	}

	return str;
}

bool InstallCommand::uploadIpa(afc_client_t afc)
{
	plist_t sinf = NULL;
	plist_t meta = NULL;
	int errp = 0;
	struct zip *zf = NULL;

	zf = zip_open(m_appid.c_str(), 0, &errp);
	if (!zf)
	{
		log_ideviceinstaller("ERROR: zip_open: %s: %d\n", m_appid.c_str(), errp);
		return false;
	}

	/* extract iTunesMetadata.plist from package */
	char *zbuf = NULL;
	uint32_t len = 0;
	plist_t meta_dict = NULL;

	if (zip_get_contents(zf, ITUNES_METADATA_PLIST_FILENAME, 0, &zbuf, &len) == true)
	{
		meta = plist_new_data(zbuf, len);
		if (memcmp(zbuf, "bplist00", 8) == 0)
		{
			plist_from_bin(zbuf, len, &meta_dict);
		}
		else
		{
			plist_from_xml(zbuf, len, &meta_dict);
		}
	}
	else{
		log_ideviceinstaller("WARNING: could not locate %s in archive!\n", ITUNES_METADATA_PLIST_FILENAME);
	}

	if (zbuf)
	{
		free(zbuf);
	}

	/* determine .app directory in archive */
	zbuf = NULL;
	len = 0;
	plist_t info = NULL;
	char filename[256];
	filename[0] = '\0';
	char* app_directory_name = NULL;
	if(!zip_get_app_directory(zf, &app_directory_name))
	{
		log_ideviceinstaller("Unable to locate app directory in archive");
		return false;
	}

/*
	for (int i = 0; i < 2; i++)
	{
		if (zip_get_app_directory(zf, &app_directory_name)){
			log_ideviceinstaller("zip_get_app_directory -- succeed");
			break;
		}
		else{
			log_ideviceinstaller("zip_get_app_directory -- failed");
#ifdef WIN32
			// 'D' attribute is missing in the folder's description. trying to zip-unzip.
			log_ideviceinstaller("Unable to locate app directory in archive after %d attemp(s)!\n", i +1);
			if (i == 0){
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				log_ideviceinstaller("WARNING: about to zip and unzip the file\n");
				string zipExePathStr = "7z";
				//zipExePathStr = "C:/Program Files (x86)/7-Zip/7z.exe";
				zipExePathStr = "C:/nexperience/7za.exe";

				size_t lastPathSeperatorIndex = m_appid.find_last_of("/\\");
				string folderPathStr = m_appid.substr(0, lastPathSeperatorIndex + 1);
				string payloadStr = folderPathStr + "Payload";
				string unzipStr = zipExePathStr + " x -y \"" + m_appid + "\" -o\"" + folderPathStr + "\"";
				string zipStr = zipExePathStr + " a -tzip \"" + m_appid + "\" \"" + payloadStr + "\"";
				string remove = "rmdir \"" + payloadStr + "\" /s /q";

				std::wstring unzipLPCWSTR(s2ws(replaceChar(unzipStr, '\\', '/')).c_str());
				std::wstring zipLPCWSTR(s2ws(replaceChar(zipStr, '\\', '/')).c_str());
				// end of strings

				// zip-unzip actions
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				//unzip
				if (!CreateProcess(NULL, &unzipLPCWSTR[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
					printf("CreateProcess for unzipping failed (%d).\n", GetLastError());
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);

				// delete old zip file
				zip_close(zf);
				int lenA = lstrlenA(m_appid.c_str());
				BSTR appidUnicodeStr = NULL;
				int lenW = ::MultiByteToWideChar(CP_ACP, 0, m_appid.c_str(), lenA, 0, 0);
				if (lenW > 0){
					appidUnicodeStr = ::SysAllocStringLen(0, lenW);
					::MultiByteToWideChar(CP_ACP, 0, m_appid.c_str(), lenA, appidUnicodeStr, lenW);
				}
				DeleteFile(appidUnicodeStr);
				::SysFreeString(appidUnicodeStr);

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				// zip again
				if (!CreateProcess(NULL, &zipLPCWSTR[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
					printf("CreateProcess for zipping failed (%d).\n", GetLastError());
				}
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);

				system(remove.c_str());

				//re-open the zip file
				zf = zip_open(m_appid.c_str(), 0, &errp);
			}
			else if (i == 1){
				return false;
			}
#endif
		}
	}

	*/

	if (zbuf)
	{
		free(zbuf);
	}
	zbuf = NULL;

	/* construct full filename to Info.plist */
	strcpy(filename, app_directory_name);
	free(app_directory_name);
	app_directory_name = NULL;
	strcat(filename, "Info.plist");

	if (zip_get_contents(zf, filename, 0, &zbuf, &len) == false)
	{
		log_ideviceinstaller("WARNING: could not locate %s in archive!\n", filename);
		zip_unchange_all(zf);
		zip_close(zf);
		return false;
	}
	if (memcmp(zbuf, "bplist00", 8) == 0)
	{
		plist_from_bin(zbuf, len, &info);
	}
	else
	{
		plist_from_xml(zbuf, len, &info);
	}
	free(zbuf);

	if (!info)
	{
		log_ideviceinstaller("Could not parse Info.plist!\n");
		zip_unchange_all(zf);
		zip_close(zf);
		return false;
	}

	char *bundleexecutable = NULL;

	plist_t bname = plist_dict_get_item(info, "CFBundleExecutable");
	if (bname)
	{
		plist_get_string_val(bname, &bundleexecutable);
	}

	bname = plist_dict_get_item(info, "CFBundleIdentifier");
	if (bname)
	{
		plist_get_string_val(bname, &m_bundleidentifier);
	}
	plist_free(info);
	info = NULL;

	if (!bundleexecutable)
	{
		log_ideviceinstaller("Could not determine value for CFBundleExecutable!\n");
		zip_unchange_all(zf);
		zip_close(zf);
		return false;
	}

	char *sinfname = NULL;
	if (asprintf(&sinfname, "Payload/%s.app/SC_Info/%s.sinf", bundleexecutable, bundleexecutable) < 0)
	{
		log_ideviceinstaller("Out of memory!?\n");
		return false;
	}
	free(bundleexecutable);

	/* extract .sinf from package */
	zbuf = NULL;
	len = 0;
	if (zip_get_contents(zf, sinfname, 0, &zbuf, &len) == true)
	{
		sinf = plist_new_data(zbuf, len);
	}
	else
	{
		log_ideviceinstaller("WARNING: could not locate %s in archive!\n", sinfname);
	}
	free(sinfname);
	if (zbuf)
	{
		free(zbuf);
	}

	/* copy archive to device */
	if (asprintf(&m_pkgname, "%s/%s", PKG_PATH, m_bundleidentifier) < 0)
	{
		log_ideviceinstaller("Out of memory!?\n");
		return false;
	}

	log_ideviceinstaller("Copying '%s' to device... ", m_appid.c_str());

	if (!afc_upload_file(afc, m_appid.c_str(), m_pkgname))
	{
		return false;
	}

	log_ideviceinstaller("DONE.\n");

	if (m_bundleidentifier)
	{
		instproxy_client_options_add(m_client_opts, "CFBundleIdentifier", m_bundleidentifier, NULL);
	}
	if (sinf)
	{
		instproxy_client_options_add(m_client_opts, "ApplicationSINF", sinf, NULL);
	}
	if (meta)
	{
		instproxy_client_options_add(m_client_opts, "iTunesMetadata", meta, NULL);
	}
	if (zf)
	{
		zip_unchange_all(zf);
		zip_close(zf);
	}

	return true;
}

char* InstallCommand::bundleIdentifier()
{
	return m_bundleidentifier == NULL ? (char*)"" : m_bundleidentifier;
}

bool InstallCommand::copyAppOnDevice(Services* service)
{
	struct stat fst;
	if (m_appid.empty())
	{
		log_ideviceinstaller("ERROR: application path not valid");
		return false;
	}
	if (stat(m_appid.c_str(), &fst) != 0)
	{
		char* a = strerror(errno);
		log_ideviceinstaller("ERROR: stat: %s: %s\n", m_appid.c_str(), a);
		return false;
	}

	char **strs = NULL;
	if (afc_get_file_info(service->getAfcClient(), PKG_PATH, &strs) != AFC_E_SUCCESS)
	{
		if (afc_make_directory(service->getAfcClient(), PKG_PATH) != AFC_E_SUCCESS)
		{
			log_ideviceinstaller("WARNING: Could not create directory '%s' on device!\n", PKG_PATH);
		}
	}
	if (strs)
	{
		int i = 0;
		while (strs[i])
		{
			free(strs[i]);
			i++;
		}
		free(strs);
	}

	if (S_ISDIR(fst.st_mode))
	{
		bool res = uploadDeveloperApp(service->getAfcClient());
		if (!res)
			return false;
	}
	else
	{
		bool res = uploadIpa(service->getAfcClient());
		if (!res)
			return false;
	}

	return true;
}
bool InstallCommand::verifyAfterInstall(Services* service)
{
	bool res = false;

	/*if (ipc)
	{
	instproxy_client_free(ipc);
	ipc = NULL;
	}
	if (LOCKDOWN_E_SUCCESS != lockdownd_client_new_with_handshake(phone, &client, "ideviceinstaller"))
	{
	log_ideviceinstaller("Could not connect to lockdownd. Exiting.\n");
	res = -1;
	//goto leave_cleanup;
	}
	if (service)
	{
	lockdownd_service_descriptor_free(service);
	}
	service = NULL;

	if ((lockdownd_start_service(client, "com.apple.mobile.installation_proxy", &service) != LOCKDOWN_E_SUCCESS) || !service)
	{
	log_ideviceinstaller("Could not start com.apple.mobile.installation_proxy!\n");
	res = -1;
	//goto leave_cleanup;
	}*/

	//instproxy_client_new(phone, service, &ipc);
	if (m_client_opts)
		plist_free(m_client_opts);
	m_client_opts = instproxy_client_options_new();
	instproxy_client_options_add(m_client_opts, "ApplicationType", "User", NULL);
	plist_t apps = NULL;

	instproxy_error_t err = instproxy_browse(service->getInstproxyClient(), m_client_opts, &apps);
	if (err != INSTPROXY_E_SUCCESS) {
		log_ideviceinstaller("ERROR: instproxy_browse returned %d\n", err);
		return res;
	}

	int apps_number_new = plist_array_get_size(apps);
	log_ideviceinstaller("apps_number_new %d\n", apps_number_new);

	for (int i = 0; i < apps_number_new; i++)
	{
		plist_t app = plist_array_get_item(apps, i);
		plist_t p_appid = plist_dict_get_item(app, "CFBundleIdentifier");
		char *s_appid = NULL;

		if (p_appid)
		{
			plist_get_string_val(p_appid, &s_appid);
		}
		if (!s_appid)
		{
			log_ideviceinstaller("ERROR: Failed to get APPID!\n");
			break;
		}
		else
		{
			log_ideviceinstaller("s_appid %s\n", s_appid);
			if (m_bundleidentifier){
				if (strcmp(s_appid, m_bundleidentifier) == 0)
				{
					res = true;
					break;
				}
			}
		}
		free(s_appid);
	}
	plist_free(apps);

	return res;
}

bool  InstallCommand::installAndVerify(Services* service, instproxy_status_cb_t status_cb, void *user_data)
{

	instproxy_error_t res = instproxy_install(service->getInstproxyClient(), m_pkgname, m_client_opts, status_cb, user_data);

	//need to wait for all notifications from device!!!!
	notification_expected = 1;
	//wait_for_op_complete = 1;
	//do_wait_when_needed();
	/* After install: check if app (bundle id) exists on device.
	* If yes - installation successful, no - failed.
	* Some applications do not display "Install - Complete" although they installed (NP-13071)
	*/
	bool result = res ? false : true;
	if (res != INSTPROXY_E_SUCCESS)
	{
		if (m_bundleidentifier)
		{
			result = verifyAfterInstall(service);
		}
		else
		{
			log_ideviceinstaller("install return false, but can not verify. bundleidentifier %s\n", m_bundleidentifier);
		}
	}
	return result;
}

bool _iequals(const char* a, const char* b)
{
	unsigned int a_len = strlen(a);
	if (strlen(b) != a_len)
		return false;
	for (unsigned int i = 0; i < a_len; ++i)
	if (tolower(a[i]) != tolower(b[i]))
		return false;
	return true;
}

string UninstallCommand::getAppIdFromAppName(Services* service, string app_name)
{
	plist_t client_opts = instproxy_client_options_new();
	instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
	plist_t apps = NULL;
	instproxy_error_t err = instproxy_browse(service->getInstproxyClient(), client_opts, &apps);
	string result = "";

	if (err != INSTPROXY_E_SUCCESS) {
		instproxy_client_options_free(client_opts);
		log_ideviceinstaller("ERROR: instproxy_browse returned %d\n", err);
		return result;
	}

	char* s_appid = NULL;
	char* s_dispName = NULL;
	char* s_bundleName = NULL;
	for (int i = 0; i < plist_array_get_size(apps); i++)
	{
		plist_t app = plist_array_get_item(apps, i);
		if (app == NULL)
		{
			log_ideviceinstaller("ERROR: APP is NULL!\n");
			continue;
		}
		plist_t p_appid = plist_dict_get_item(app, "CFBundleIdentifier");
		plist_t p_dispName = plist_dict_get_item(app, "CFBundleDisplayName");
		plist_t p_bundleName = plist_dict_get_item(app, "CFBundleName");
		s_appid = NULL;
		s_dispName = NULL;
		if (p_appid)
		{
			plist_get_string_val(p_appid, &s_appid);
		}
		if (!s_appid)
		{
			log_ideviceinstaller("ERROR: Failed to get APPID!\n");
			break;
		}
		if (p_bundleName)
		{
			plist_get_string_val(p_bundleName, &s_bundleName);
		}
		if (p_dispName)
		{
			plist_get_string_val(p_dispName, &s_dispName);
		}
		if (!s_dispName)
		{
			s_dispName = strdup(s_appid);
		}
		if (_iequals(s_dispName, app_name.c_str()) || (s_bundleName != NULL && _iequals(s_bundleName, app_name.c_str())))
		{
			result = s_appid;
			break;
		}
		free(s_dispName);
		s_dispName = NULL;
		free(s_appid);
		s_appid = NULL;
		free(s_bundleName);
		s_bundleName = NULL;
	}
	if (s_dispName)
		free(s_dispName);
	if (s_appid)
		free(s_appid);
	if (s_bundleName)
		free(s_bundleName);
	plist_free(apps);
	instproxy_client_options_free(client_opts);
	return result;
}

bool  UninstallCommand::uninstall(Services* service)
{
	if (m_appid.empty() || m_appid == "null")
		m_appid = getAppIdFromAppName(service, m_app_name);
	if (m_appid.empty())
		return false;
	instproxy_error_t res = instproxy_uninstall(service->getInstproxyClient(), m_appid.c_str(), NULL, NULL, NULL);
	notification_expected = 1;
	return res ? false : true;
}
