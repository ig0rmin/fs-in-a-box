#include "InprocSrv.h"
#include "Fs.h"
#include "FsClassFactory.h"

#include <new>
#include <string>

static long gObjCount = 0;
static long gServerLocks = 0;
static HMODULE gDllHandle = 0;

static const std::string gProgId = "FsBox.Fs";

void IncrementObjCount()
{
	InterlockedIncrement(&gObjCount);
}

void DecrementObjCount()
{
	InterlockedDecrement(&gObjCount);
}

void IncrementServerLocks()
{
	InterlockedIncrement(&gServerLocks);
}

void DecrementServerLocks()
{
	InterlockedDecrement(&gServerLocks);
}

STDAPI DllCanUnloadNow()
{
	if (!gObjCount && !gServerLocks)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

void SetModuleHandle(HMODULE hmodule)
{
	gDllHandle = hmodule;
}

static std::wstring GetFsBoxCLSID()
{
	LPOLESTR olestr_CLSID_FsBox = 0;
	if (FAILED(StringFromCLSID(CLSID_FsBox, &olestr_CLSID_FsBox)))
	{
		return L"";
	}
	std::wstring result(olestr_CLSID_FsBox, olestr_CLSID_FsBox + wcslen(olestr_CLSID_FsBox));
	CoTaskMemFree(olestr_CLSID_FsBox);
	return result;
}

STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
	if (clsid != CLSID_FsBox)
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}
	FsClassFactory* fsClassFactory = new(std::nothrow) FsClassFactory();
	if (!fsClassFactory)
	{
		return E_OUTOFMEMORY;
	}
	HRESULT hr = fsClassFactory->QueryInterface(iid, ppv);
	fsClassFactory->Release();
	return hr;
}

STDAPI DllRegisterServer()
{
	std::wstring strClsid = L"CLSID\\" + GetFsBoxCLSID();
	// Create HKEY_CLASSES_ROOT\CLSID\{CLSID_FsBox} subkey
	HKEY keyClsid = nullptr;
	LSTATUS res = RegCreateKeyExW(HKEY_CLASSES_ROOT, strClsid.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &keyClsid, nullptr);
	if (ERROR_SUCCESS != res)
	{
		return E_FAIL;
	}
	res = RegSetValueExA(keyClsid, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(gProgId.c_str()), gProgId.size());
	if (ERROR_SUCCESS != res)
	{
		return E_FAIL;
	}
	// Create InprocServer32 subkey
	HKEY keyInprocServer32 = 0;
	res = RegCreateKeyExW(keyClsid, L"InprocServer32", 0, nullptr, 0, KEY_WRITE, nullptr, &keyInprocServer32, nullptr);
	RegCloseKey(keyClsid);
	if (ERROR_SUCCESS != res)
	{
		return E_FAIL;
	}
	// Set  InprocServer32 valude
	char moduleName[MAX_PATH] = {0};
	DWORD moduleNameSize = GetModuleFileNameA(gDllHandle, moduleName, MAX_PATH);
	if (!moduleNameSize)
	{
		RegCloseKey(keyClsid);
		return E_FAIL;
	}
	res = RegSetValueExA(keyInprocServer32, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(moduleName), moduleNameSize + 1);
	RegCloseKey(keyInprocServer32);
	return (ERROR_SUCCESS == res) ? S_OK : E_FAIL;
}

STDAPI DllUnregisterServer()
{
	std::wstring strClsid = L"CLSID\\" + GetFsBoxCLSID();
	LSTATUS res = RegDeleteTreeW(HKEY_CLASSES_ROOT, strClsid.c_str());
	return (ERROR_SUCCESS == res) ? S_OK : E_FAIL;;
}