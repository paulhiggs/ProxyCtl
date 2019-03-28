/*
  https://msdn.microsoft.com/en-us/library/windows/desktop/aa385148(v=vs.85).aspx
 */
 

#include <string>

#include <windows.h>
#include <wininet.h>

#include "ProxyState.h"
#include "SysError.h"

#undef _EXPERIMENTAL_
#undef __USE_WINDOWS_INTERNET_LIBRARY__

#include <iostream>

using namespace std;

const char *ProxyKeyLocation = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
const char *ProxyServerKey = "ProxyServer";
const char *ProxyEnableKey = "ProxyEnable";
const char *ProxyBypassKey = "ProxyOverRide";


ProxyState::ProxyState()
{
	verbose = false;
	enabled = false;
	strcpy(url,  "");
}


ProxyState::~ProxyState()
{
}

bool ProxyState::isVerbose(void)
{
	return verbose;
}

#ifdef __USE_WINDOWS_INTERNET_LIBRARY__
LONG readInternetOption(char *u, bool *e)
{
	INTERNET_PROXY_INFO proxyInfo;
	proxyInfo.lpszProxy = (char *)malloc(1024);
	proxyInfo.lpszProxyBypass = (char *)malloc(1024);
	DWORD lenProxyInfo = sizeof(proxyInfo) + 1024 + 1024;

	if (!InternetQueryOption(NULL /* global settings */, INTERNET_OPTION_PROXY, (LPVOID)&proxyInfo, &lenProxyInfo)) {
		DWORD err = GetLastError();
		if (err = ERROR_INSUFFICIENT_BUFFER)
			cerr << "buffer to small: need=" << lenProxyInfo << endl;
		return err;
	}

	strcpy(u, proxyInfo.lpszProxy);
	*e = (proxyInfo.dwAccessType != INTERNET_OPEN_TYPE_DIRECT) ? true : false;

	return ERROR_SUCCESS;
}

LONG ProxyState::readProxyState(void)
{
	if (verbose)
		cout << ">>readProxyState()" << endl;

	readInternetOption((char *)&url, &enabled);

	cout << "enabled = " << ((enabled) ? "true" : "false") << endl;

	/*
	strcpy(url, proxyInfo.lpszProxy);
	enabled = (proxyInfo.dwAccessType != INTERNET_OPEN_TYPE_DIRECT) ? true : false;
	*/

	return ERROR_SUCCESS;
}
#else

LONG readKey(HKEY hkeyDXVer, const char *key, char *result, DWORD resultLen) {
	char Buffer[50 * MAX_PATH];
	DWORD BufferSize, dwBuffer, Type;
	LONG  lResult;

	strcpy(Buffer, ""); BufferSize = sizeof(Buffer);
	if ((lResult = RegQueryValueEx(hkeyDXVer, key, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&Buffer,
		&BufferSize)
		) == ERROR_SUCCESS)
	{
		if (Type == REG_SZ) { strncpy(result, Buffer, resultLen); }
	}
	else
	{
		// report the RegQueryValueEx() error
		OSErrorMan e(lResult);
		cerr << "Error in RegOpenKeyEx(" << key << "): " << e << endl;
		return lResult;
	}
	return ERROR_SUCCESS;
}
LONG ProxyState::readProxyState(void)
{
	if (verbose)
		cout << ">>readProxyState()" << endl;

	HKEY hkeyDXVer;
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, ProxyKeyLocation, 0, KEY_READ, &hkeyDXVer);
	if (lResult != ERROR_SUCCESS) {
		OSErrorMan e(lResult);
		cerr << "Error in RegOpenKeyEx(): " << e << endl;
		return lResult;
	}

	// fetch HKCU/Software/Microsoft/Windows/CurrentVersion/ProxyEnable into enabled
	char Buffer[MAX_PATH];
	DWORD dwBuffer;
	DWORD Type, BufferSize;

	dwBuffer = 0; BufferSize = sizeof(dwBuffer);
	if ((lResult = RegQueryValueEx(hkeyDXVer, ProxyEnableKey, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&dwBuffer,
		&BufferSize)
		) == ERROR_SUCCESS)
	{
		if (Type == REG_DWORD) { enabled = (dwBuffer == 1); }
	}
	else
	{
		// report the RegQueryValueEx() error
		OSErrorMan e(lResult);
		cerr << "Error in RegOpenKeyEx(" << ProxyEnableKey << "): " << e << endl;
		return lResult;
	}

	if ((lResult = readKey(hkeyDXVer, ProxyServerKey, url, sizeof(url))) != ERROR_SUCCESS)
		return lResult;
	/*
	strcpy(Buffer, ""); BufferSize = sizeof(Buffer);
	if ((lResult = RegQueryValueEx(hkeyDXVer, ProxyServerKey, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&Buffer,
		&BufferSize)
		) == ERROR_SUCCESS)
	{
		if (Type == REG_SZ) { strncpy(url, Buffer, sizeof(url)); }
	}
	else 
	{
		// report the RegQueryValueEx() error
		OSErrorMan e(lResult);
		cerr << "Error in RegOpenKeyEx(" << ProxyServerKey << "): " << e << endl;
		return lResult;
	}
	*/
	if ((lResult = readKey(hkeyDXVer, ProxyBypassKey, bypass, sizeof(bypass))) != ERROR_SUCCESS)
		return lResult;
	/*
	strcpy(Buffer, ""); BufferSize = sizeof(Buffer);
	if ((lResult = RegQueryValueEx(hkeyDXVer, ProxyBypassKey, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&Buffer,
		&BufferSize)
		) == ERROR_SUCCESS)
	{
		if (Type == REG_SZ) { strncpy(bypass, Buffer, sizeof(bypass)); }
	}
	else
	{
		// report the RegQueryValueEx() error
		OSErrorMan e(lResult);
		cerr << "Error in RegOpenKeyEx(" << ProxyBypassKey << "): " << e << endl;
		return lResult;
	}
	*/
	return ERROR_SUCCESS;
}


#endif

/*
ostream& operator<<(ostream& ost, const ProxyState& ps)
{
	//	ost << "~|" << ls.time_string << '|' << ls.data << "|~";
	ost << (ps.isEnabled() ? "enabled" : "disabled") << "; url: " << ps.getUrl() << endl;
	return ost;
}
*/

DWORD signalApplications()
{
	return (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) ? GetLastError() : ERROR_SUCCESS;

	/*
	DWORD_PTR dwret;
	return (SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL, SMTO_NORMAL, 1000, &dwret) == 0) ? GetLastError() : ERROR_SUCCESS;
	*/
}



int ProxyState::setUrl(char *newurl)
{
	if (verbose)
		cout << "setUrl(" << newurl << ")" << endl;

	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, ProxyKeyLocation, 0, KEY_SET_VALUE, &hkeyDXVer);
	if (lResult != ERROR_SUCCESS) {

		return lResult;
	}
	char *x = strdup(newurl);
	LONG err;

	if ((err = RegSetValueEx(hkeyDXVer, ProxyServerKey, NULL,
		REG_SZ,
		(LPCBYTE)x,
		strlen(x)
	)) == ERROR_SUCCESS) {
		strncpy(url, newurl, sizeof(url));
		
		if ((err = signalApplications() != ERROR_SUCCESS)) {
			OSErrorMan e(err);
			cerr << "Error Signalling Applications: err=" << e << endl;
		}
	}
	free(x);
	return err;
}


int ProxyState::setEnabled(bool state)
{
	if (verbose)
		cout << "setEnabled(" << state << ")" << endl;

	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, ProxyKeyLocation, 0, KEY_SET_VALUE, &hkeyDXVer);

	DWORD dwBuffer = (state) ? 1 : 0, 
		  BufferSize = sizeof(dwBuffer);
	LONG err;

	if ((err = RegSetValueEx(hkeyDXVer, ProxyEnableKey, NULL,
		REG_DWORD,
		(LPBYTE)&dwBuffer,
		BufferSize
	)) == ERROR_SUCCESS) {
			enabled = state;
			if ((err = signalApplications()) != ERROR_SUCCESS) {
				OSErrorMan e(err);
				cerr << "Error Signalling Applications: err=" << e << endl;
			};
	}
	return err;
}
