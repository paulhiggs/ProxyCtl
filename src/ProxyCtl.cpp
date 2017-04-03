// ProxyCtl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>

#include "ProxyState"

int main(int argc, char *argv[])
{
	/* check args: appname [ /e[nable] | /d[isble] ] [url]
	*/

	bool doEnable = false, doDisable = false;
	char url[MAX_PATH] = "";

	if (argc == 1) /* no args, show current state */
	{
		ProxyState p;
	
		p.fetch();
		printf("%s; url: %s", p.enabled?"enabled":"disabled", p.url)
	}
	else {

	}

    return 0;
}

