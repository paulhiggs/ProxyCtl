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


#define VERSION "0.3"

typedef struct item_t { const char *name; const char *value; const char *description;  } item_t;
item_t known_proxy_table[] = {
	{ "au", "proxyau.huawei.com:8080", "Australia" },
    { "ba", "proxybh.huawei.com:8080", "Bahrain" },
    { "br", "proxybh.huawei.com:8080", "Brazil" },
	{ "ch", "proxych.huawei.com:8080", "Switzerland" },
	{ "cn", "proxycn2.huawei.com:8080", "China" },   
	{ "de", "proxyde.huawei.com:8080", "Germany" },
	{ "fi", "proxyfi.huawei.com:8080", "Finland" },
	{ "hk", "proxyhk.huawei.com:8080", "Hong Kong" },
	{ "in", "proxyblr.huawei.com:8080", "India" },
	{ "jp", "proxyjp.huawei.com:8080", "Japan" },
	{ "nj", "proxynj.huawei.com:8080", "Nanjing" },
	{ "no", "proxych.huawei.com:8080", "Norway" },
	{ "ru", "proxyru.huawei.com:8080", "Russia" },
	{ "sz", "proxysz.huawei.com:8080", "Shenzhen" },
	{ "uk", "proxyuk.huawei.com:8080", "UK" },
	{ "us", "proxyus.huawei.com:8080", "USA" },
{ NULL, NULL, NULL }
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
	cout << "version: "<< VERSION << endl << "usage: " << progname << " [-h | [-c] [-e | -d] [-p <abbr>] [<proxyHost>]]" << endl;
	
	int i = 0;
	while (known_proxy_table[i].name != NULL) {
		if (i == 0) cout << "known proxies:" << endl;
		cout << "  " << known_proxy_table[i].name << "(" << known_proxy_table[i].description << ")" << "-->" << known_proxy_table[i].value << endl;
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
		bool showConfig = false, argEnable = false, argDisable = false, argsOK = true, needHelp = false, abortArgs = false;

		while ( !abortArgs && ((c = getopt(argc, argv, "cvhedp:")) != EOF))
		{
			switch (c)
			{
			case 'c':
				showConfig = true;
				break;
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
				cout << "invalid argument " << argv[optind-1] << endl;
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

			if (p.isVerbose() && !argEnable && !argDisable && !knownHost && !specifiedHost && !showConfig)
				cout << (p.isEnabled() ? "enabled" : "disabled") << "; url: " << p.getUrl() << endl;

			if (showConfig) {
				cout << (p.isEnabled() ? "enabled" : "disabled") << "; url: " << p.getUrl() << endl;
				cout << "  bypass: " << p.getBypass() << endl;
			}
		}
		else
			usage(argv[0]);
	}

    return 0;
}

