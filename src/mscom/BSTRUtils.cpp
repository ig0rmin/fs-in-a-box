#include "BSTRUtils.h"

#include <locale>
#include <codecvt>

#include <OleAuto.h>

std::string BSTRToString(BSTR bstr)
{
	std::wstring wStr(bstr, SysStringLen(bstr));
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::string utf8Str = convert.to_bytes(wStr);
	return utf8Str;
}

BSTR StringToBSTR(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::wstring wStr = convert.from_bytes(str);
	return SysAllocStringLen(wStr.c_str(), wStr.size());
}