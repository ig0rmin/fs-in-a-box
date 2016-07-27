

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Jul 27 19:24:14 2016
 */
/* Compiler settings for FS.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IFs,0x9ffa7dea,0x5411,0x11e6,0x89,0x2f,0x08,0x00,0x27,0x66,0xfd,0xaa);


MIDL_DEFINE_GUID(IID, IID_IDirectoryEnumerator,0x5d8f7fa2,0x5414,0x11e6,0x9d,0x34,0x08,0x00,0x27,0x66,0xfd,0xaa);


MIDL_DEFINE_GUID(IID, IID_IDirectory,0x8719e9ee,0x5413,0x11e6,0xa2,0x8c,0x08,0x00,0x27,0x66,0xfd,0xaa);


MIDL_DEFINE_GUID(IID, IID_IFile,0x45bea892,0x5416,0x11e6,0xbe,0x20,0x08,0x00,0x27,0x66,0xfd,0xaa);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



