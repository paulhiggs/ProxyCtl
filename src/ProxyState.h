#pragma once
#include <string>

using namespace std;

#define MAX_URL_LEN MAX_PATH*4

class ProxyState
{
public:
	ProxyState();
	~ProxyState();

//	friend ostream& operator<<(ostream& ost, const ProxyState& ps);

private:
	bool enabled;
	bool verbose;
	char url[MAX_URL_LEN];

public:
	void setVerbose(bool argVerbose) { verbose = argVerbose; };
	bool isVerbose(void);
	bool isEnabled() { return enabled; };
	int setUrl(char *newurl);
	string getUrl() { return url; };
	int setEnabled(bool state);

	LONG readProxyState(void);
};

