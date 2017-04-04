#include "syserror.h"


OSErrorMan::OSErrorMan()
{
	eNo = ERROR_SUCCESS;
	strcpy(eStr,"");
}

OSErrorMan::OSErrorMan(DWORD errorNum)
{
	eNo = errorNum;
	strcpy(eStr, "");
}


OSErrorMan::~OSErrorMan()
{
}




ostream& operator<<(ostream& os, const OSErrorMan& e)
{
	DWORD err2;

	err2 = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		e.eNo,
		LANG_USER_DEFAULT,
		(LPSTR)&(e.eStr),
		MAX_ERROR_STR_LEN,
		NULL
	);

	if (err2 == 0) {
		os << "Can't FormatMessage(" << e.eNo << ")=" << GetLastError();
	}
	else {
		os << e.eStr;
	}

	return os;
}

