#pragma once

#include <windows.h>
#include <iostream>  

using namespace std;

#define MAX_ERROR_STR_LEN	256

class OSErrorMan 
{
public:
	OSErrorMan();
	OSErrorMan(DWORD errorNum);
	~OSErrorMan();

	friend ostream& operator<<(ostream& os, const OSErrorMan& e);

private:
	DWORD eNo;
	char eStr[MAX_ERROR_STR_LEN];
};