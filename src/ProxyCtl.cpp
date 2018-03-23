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


#define VERSION "0.2"

typedef struct item_t { const char *name; const char *value; } item_t;
item_t known_proxy_table[] = {
	{ "us", "proxyus3.huawei.com:8080" },
	{ "au", "proxyau.huawei.com:8080" },
	{ "uk", "proxyuk.huawei.com:8080" },
	{ "de", "proxyde.huawei.com:8080" },
	{ "sz", "proxysz.huawei.com:8080" },
	{ "hk", "proxyhk.huawei.com:8080" },
	{ NULL, NULL }
};

const char * proxylookup(const char *name)
{
	for (item_t *p = known_proxy_table; p->name != NULL; ++p) {
		if (stricmp(p->name, name) == 0) {
			return p->value;
		}
	}
	return NULL;
}


void usage(char *progname) {
	cout << "version: "<< VERSION << endl << "usage: " << progname << " [-h | [-e | -d] [-p <abbr>] [<proxyHost>]]" << endl;
	
	int i = 0;
	while (known_proxy_table[i].name != NULL) {
		if (i == 0) cout << "known proxies:" << endl;
		cout << "  " << known_proxy_table[i].name << "-->" << known_proxy_table[i].value << endl;
		i++;
	}
}




int main(int argc, char *argv[])
{
	bool doEnable = false, doDisable = false;
	// char url[MAX_PATH] = "";
	char *preferredHost = NULL, *specifiedHost = NULL, *knownHost = NULL;

	ProxyState p;

	if (argc == 1) /* no args, show current state */
	{
		DWORD err = p.readProxyState();
		if (err == ERROR_SUCCESS) {
			cout << (p.isEnabled() ? "enabled" : "disabled") << "; url: " << p.getUrl() << endl;
		}
		else {
			OSErrorMan e(err);
			cerr << "readProxyState() failed: " << e << endl;
		}
	}
	else {
		int c;
		optind = 0;		// this does not normally have to be done, but in
						// this app we may be calling getopt again
		bool argEnable=false, argDisable=false, argsOK=true, needHelp=false, abortArgs=false;

		while ( !abortArgs && ((c = getopt(argc, argv, "vhedp:")) != EOF))
		{
			switch (c)
			{
			case 'v':
				p.setVerbose(true);
				break;
			case 'h':
				needHelp = true;
				abortArgs = true;
				break;
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
				else {
					argsOK = false;
					cout << "no prefedined host" << endl;
				}
				break;
			default:
				cout << "invalud argument " << argv[optind-1] << endl;
				argsOK = false;
				break;
			}
		}

		if (needHelp) {
			usage(argv[0]);
			return 0;
		}

		if (optind < argc) {
			/* remaining arguments should be proxy host names - but we just need 1 */
			specifiedHost = strdup(argv[optind]);
		}


		if (argEnable && argDisable) {
			/* cannot enable and disable - abort*/
			cout << "specify just one of -e or -d" << endl;
			argsOK = false;
		}
		

		if (preferredHost) {
			// check validity - use if valid
			knownHost = (char *)proxylookup(preferredHost);
			if (knownHost == NULL) {
				preferredHost = NULL;
				argsOK = false;
			}
			else {
				specifiedHost = knownHost;
			}
		}

		if (argsOK) {
			DWORD err = p.readProxyState();
			if (err != ERROR_SUCCESS) {
				OSErrorMan e(err);
				cerr << "readProxyState() failed: " << e << endl;
			}
			if (knownHost) {
				DWORD err;
				if ((err = p.setUrl(knownHost)) != ERROR_SUCCESS) {
					OSErrorMan e(err);
					cout << "error setting proxy(" << knownHost << "), error=" << e << endl;
				}
				else cout << "proxy set to " << knownHost << endl;
			}
			else if (specifiedHost) {
				DWORD err;
				if ((err = p.setUrl(specifiedHost)) != ERROR_SUCCESS) {
					OSErrorMan e(err);
					cout << "error setting proxy(" << specifiedHost << "), error=" << e << endl;
				}
				else cout << "proxy set to " << specifiedHost << endl;
			}

			if (argEnable) {
				DWORD err;
				if ((err = p.setEnabled(true)) != ERROR_SUCCESS) {
					/* couldn't set registry to true*/
					OSErrorMan e(err);
					cout << "error enabling proxy, error=" << e << endl;
				}
				else cout << "proxy enabled" << endl;
			}
			if (argDisable) {
				DWORD err;
				if ((err = p.setEnabled(false)) != ERROR_SUCCESS) {
					/* couldn't set registry to false*/
					OSErrorMan e(err);
					cout << "error disabling proxy, error=" << e << endl;
				}
				else cout << "proxy disabled" << endl;
			}

			if (p.isVerbose() && !argEnable && !argDisable && !knownHost && !specifiedHost)
				cout << (p.isEnabled() ? "enabled" : "disabled") << "; url: " << p.getUrl() << endl;
		}
		else
			usage(argv[0]);
	}

    return 0;
}

