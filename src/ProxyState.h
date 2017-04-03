#pragma once
#include <string>

using namespace std;

class ProxyState
{
public:
	ProxyState();
	~ProxyState();


protected:
	bool enabled;
	string url;

public:
	bool isEnabled();
	int setUrl(string url);
	string getUrl();
	int setEnabled(bool state);
};

