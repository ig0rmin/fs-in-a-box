#ifndef __FSBOX_MSCOM_BSTR_UTILS_H__
#define __FSBOX_MSCOM_BSTR_UTILS_H__

#include <string>
#include <wtypes.h>
	
std::string BSTRToString(BSTR bstr);
BSTR StringToBSTR(const std::string& str);

#endif