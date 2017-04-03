#include <string>

#include <windows.h>

#include "ProxyState.h"



using namespace std;

#define BUFFER 8192

ProxyState::ProxyState()
{
	enabled = false;
	url = "";

	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hkeyDXVer);

	// fetch HKCU/Software/Microsoft/Windows/CurrentVersion/ProxyEnable into enabled
	char Buffer[BUFFER];
	DWORD dwBuffer;
	DWORD Type, BufferSize;
	LONG err; 

	dwBuffer = 0; BufferSize = sizeof(dwBuffer);

	if ((err = RegQueryValueEx(hkeyDXVer, "ProxyEnable", NULL,
		(LPDWORD)&Type,
		(LPBYTE)&dwBuffer,
		&BufferSize)
		) == ERROR_SUCCESS )
	{
		if (Type == REG_DWORD) { enabled = (dwBuffer == 1); }
		
	}



	// fetch HKCU/Software/Microsoft/Windows/CurrentVersion/ProxyServer into url
	strcpy(Buffer, ""); BufferSize = sizeof(Buffer);
	if ((err = RegQueryValueEx(hkeyDXVer, "ProxyServer", NULL,
		(LPDWORD)&Type,
		(LPBYTE)&Buffer,
		&BufferSize)
		) == ERROR_SUCCESS )
	{
		if (Type == REG_SZ) {url = Buffer; }
	}
}


ProxyState::~ProxyState()
{
}






int ProxyState::setUrl(string url)
{
	return 0;
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

	HKEY hkeyDXVer;
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_SET_VALUE, &hkeyDXVer);

	DWORD dwBuffer = (state) ? 1 : 0, 
		  BufferSize = sizeof(dwBuffer);
	LONG err;

	if ((err = RegSetValueEx(hkeyDXVer, "ProxyEnable", NULL,
		REG_DWORD,
		(LPBYTE)&dwBuffer,
		BufferSize
		)) == ERROR_SUCCESS) {

		enabled = state;
	}

	return err;
}
