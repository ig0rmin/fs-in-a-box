#ifndef __FSBOX_MSCOM_INPROC_SRV_H__
#define __FSBOX_MSCOM_INPROC_SRV_H__

#include <Unknwn.h>
#include <guiddef.h>

void IncrementObjCount();
void DecrementObjCount();

void IncrementServerLocks();
void DecrementServerLocks();

void SetModuleHandle(HMODULE hmodule);

extern "C"
{
HRESULT __stdcall DllCanUnloadNow();
HRESULT __stdcall DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv);
HRESULT __stdcall DllRegisterServer();
HRESULT __stdcall DllUnregisterServer();
}

#endif