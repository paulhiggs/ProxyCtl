// ProxyCtl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include "xgetopt.h"

#include "ProxyState.h"
#include "SysError.h"

#include <iostream>
using namespace std;


const char * lookup(const char *name)
{
	typedef struct item_t { const char *name; const char *value; } item_t;
	item_t table[] = {
		{ "us3", "proxyus3.huawei.com:8080" },
		{ "au", "proxyau.huawei.com:8080" },
		{ "de", "proxyde.huawei.com:8080" },
		{ NULL, NULL }
	};
	for (item_t *p = table; p->name != NULL; ++p) {
		if (stricmp(p->name, name) == 0) {
			return p->value;
		}
	}
	return NULL;
}


/*
char *lut[] = {
	{ "us3", "proxyus3.huawei.com", },
	{ "au", "proxyau.huawei.com", },
	{ "de", "proxyde.huawei.com", },
	0
};
*/

int main(int argc, char *argv[])
{
	/* check args: appname [ /e[nable] | /d[isble] ] [url]
	*/

	bool doEnable = false, doDisable = false;
	// char url[MAX_PATH] = "";
	char *preferredHost = NULL, *specifiedHost = NULL, *knownHost = NULL;

	ProxyState p;

	if (argc == 1) /* no args, show current state */
	{
		cout << (p.isEnabled() ? "enabled" : "disabled") << "; url: " << p.getUrl() << endl;
	}
	else {
		/* process args */

		int c;
		optind = 0;		// this does not normally have to be done, but in
						// this app we may be calling getopt again
		bool argEnable = false, argDisable = false;

		while ((c = getopt(argc, argv, "edp:")) != EOF)
		{
			switch (c)
			{
			case 'e':
				argEnable = true;
				break;
			case 'd':
				argDisable = true;
				break;
			case 'p':
				//its a pre-defined host
				if (optind <= argc) {
					preferredHost = strdup(argv[optind - 1]);
				}
				else cout << "no prefedined host" << endl;
				break;
			default:
				char cc = c;
				cout << "invalud argument " << cc << endl;
				break;
			}
		}
		if (optind < argc) {
			/* remaining arguments should be proxy host names - but we just need 1 */
			specifiedHost = strdup(argv[optind]);
		}


		if (argEnable && argDisable) {
			/* cannot enable and disable - abort*/
			cout << "specify just one of /e or /d" << endl;
		}
		else if (argEnable) {
			DWORD err;
			if ((err = p.setEnabled(true)) != ERROR_SUCCESS) {
				/* couldn't set registry to true*/
				OSErrorMan e(err);
				cout << "error enabling proxy, error=" << e << endl;
			}
		}
		else if (argDisable) {
			DWORD err;
			if ((err = p.setEnabled(false)) != ERROR_SUCCESS) {
				/* couldn't set registry to false*/
				OSErrorMan e(err);
				cout << "error disabling proxy, error=" << e << endl;
			}
		}

		if (preferredHost) {
			// check validity - use if valid
			knownHost = (char *)lookup(preferredHost);
			if (knownHost == NULL) {
				preferredHost = NULL;
			}
			else {
				specifiedHost = knownHost;
			}
		}

		if (knownHost) {
			DWORD err;
			if ((err = p.setUrl(knownHost)) != ERROR_SUCCESS) {
				OSErrorMan e(err);
				cout << "error setting proxy(" << knownHost << "), error=" << e << endl;
			}
		}
		else if (specifiedHost) {
			DWORD err;
			if ((err = p.setUrl(specifiedHost)) != ERROR_SUCCESS) {
				OSErrorMan e(err);
				cout << "error setting proxy(" << specifiedHost << "), error=" << e << endl;
			}
		}
	}

    return 0;
}

