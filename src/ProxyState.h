#pragma once
#include <string>

using namespace std;

#define MAX_URL_LEN MAX_PATH

class ProxyState
{
public:
	ProxyState();
	~ProxyState();


protected:
	bool enabled;
	char  url[MAX_URL_LEN];

public:
	bool isEnabled();
	int setUrl(char *newurl);
	string getUrl();
	int setEnabled(bool state);
};

