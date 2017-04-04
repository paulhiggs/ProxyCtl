#include <string>

#include <windows.h>

#include "ProxyState.h"

#undef _EXPERIMENTAL_

#ifdef _EXPERIMENTAL_
#include <iostream>
#endif


using namespace std;

const char *ProxyKeyLocation = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
const char *ProxyServerKey = "ProxyServer";
const char *ProxyEnableKey = "ProxyEnable";


ProxyState::ProxyState()
{
	enabled = false;
	strcpy(url,  "");

	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, ProxyKeyLocation, 0, KEY_READ, &hkeyDXVer);

	// fetch HKCU/Software/Microsoft/Windows/CurrentVersion/ProxyEnable into enabled
	char Buffer[MAX_PATH];
	DWORD dwBuffer;
	DWORD Type, BufferSize;
	LONG err; 

	dwBuffer = 0; BufferSize = sizeof(dwBuffer);
	if ((err = RegQueryValueEx(hkeyDXVer, ProxyEnableKey, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&dwBuffer,
		&BufferSize)
		) == ERROR_SUCCESS )
	{
		if (Type == REG_DWORD) { enabled = (dwBuffer == 1); }
	}

	strcpy(Buffer, ""); BufferSize = sizeof(Buffer);
	if ((err = RegQueryValueEx(hkeyDXVer, ProxyServerKey, NULL,
		(LPDWORD)&Type,
		(LPBYTE)&Buffer,
		&BufferSize)
		) == ERROR_SUCCESS )
	{
		if (Type == REG_SZ) {strncpy(url, Buffer, sizeof(url)); }
	}
}


ProxyState::~ProxyState()
{
}


int ProxyState::setUrl(char *newurl)
{
#ifdef _EXPERIMENTAL_
	cout << "setUrl(" << newurl << ")" << endl;
	return 0;
#else
	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, ProxyKeyLocation, 0, KEY_SET_VALUE, &hkeyDXVer);

	char *x = strdup(newurl);
	LONG err;

	if ((err = RegSetValueEx(hkeyDXVer, ProxyServerKey, NULL,
		REG_SZ,
		(LPCBYTE)x,
		strlen(x)
	)) == ERROR_SUCCESS) {

		strncpy(url, newurl, sizeof(url));
	}
	free(x);
	return err;
#endif
}


string ProxyState::getUrl()
{
	return url;
}

bool ProxyState::isEnabled()
{
	return enabled;
}


int ProxyState::setEnabled(bool state)
{
#ifdef _EXPERIMENTAL_
	cout << "setEnabled(" << state << ")" << endl;
	return 0;
#else
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
	}
	return err;
#endif
}
