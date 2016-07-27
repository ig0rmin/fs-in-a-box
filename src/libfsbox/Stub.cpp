// This file is created in order to test build system and will be removed later

#include "Stub.h"

#if _WIN32
	#include <windows.h>
#endif

void CallStub()
{
#if _WIN32
	MessageBoxW(GetActiveWindow(), L"HONK", L"HONK", MB_OK);
#endif
}