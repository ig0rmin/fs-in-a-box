

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __FS_h__
#define __FS_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFs_FWD_DEFINED__
#define __IFs_FWD_DEFINED__
typedef interface IFs IFs;

#endif 	/* __IFs_FWD_DEFINED__ */


#ifndef __IDirectoryEnumerator_FWD_DEFINED__
#define __IDirectoryEnumerator_FWD_DEFINED__
typedef interface IDirectoryEnumerator IDirectoryEnumerator;

#endif 	/* __IDirectoryEnumerator_FWD_DEFINED__ */


#ifndef __IDirectory_FWD_DEFINED__
#define __IDirectory_FWD_DEFINED__
typedef interface IDirectory IDirectory;

#endif 	/* __IDirectory_FWD_DEFINED__ */


#ifndef __IFile_FWD_DEFINED__
#define __IFile_FWD_DEFINED__
typedef interface IFile IFile;

#endif 	/* __IFile_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_FS_0000_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_FS_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_FS_0000_0000_v0_0_s_ifspec;

#ifndef __IFs_INTERFACE_DEFINED__
#define __IFs_INTERFACE_DEFINED__

/* interface IFs */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9ffa7dea-5411-11e6-892f-08002766fdaa")
    IFs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR fileName,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ IDirectory **root) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFs * This);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IFs * This,
            /* [in] */ BSTR fileName,
            /* [in] */ DWORD desiredAccess,
            /* [out] */ IDirectory **root);
        
        END_INTERFACE
    } IFsVtbl;

    interface IFs
    {
        CONST_VTBL struct IFsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFs_Open(This,fileName,desiredAccess,root)	\
    ( (This)->lpVtbl -> Open(This,fileName,desiredAccess,root) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFs_INTERFACE_DEFINED__ */


#ifndef __IDirectoryEnumerator_INTERFACE_DEFINED__
#define __IDirectoryEnumerator_INTERFACE_DEFINED__

/* interface IDirectoryEnumerator */
/* [local][uuid][object] */ 

typedef struct tagFileAttributes
    {
    DWORD dummy;
    } 	FileAttributes;


EXTERN_C const IID IID_IDirectoryEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5d8f7fa2-5414-11e6-9d34-08002766fdaa")
    IDirectoryEnumerator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            BSTR **name,
            FileAttributes *attributes) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDirectoryEnumeratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirectoryEnumerator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirectoryEnumerator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirectoryEnumerator * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IDirectoryEnumerator * This,
            BSTR **name,
            FileAttributes *attributes);
        
        END_INTERFACE
    } IDirectoryEnumeratorVtbl;

    interface IDirectoryEnumerator
    {
        CONST_VTBL struct IDirectoryEnumeratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectoryEnumerator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDirectoryEnumerator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDirectoryEnumerator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDirectoryEnumerator_Next(This,name,attributes)	\
    ( (This)->lpVtbl -> Next(This,name,attributes) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDirectoryEnumerator_INTERFACE_DEFINED__ */


#ifndef __IDirectory_INTERFACE_DEFINED__
#define __IDirectory_INTERFACE_DEFINED__

/* interface IDirectory */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IDirectory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8719e9ee-5413-11e6-a28c-08002766fdaa")
    IDirectory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IDirectory **parent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenDirectory( 
            /* [in] */ BSTR name,
            /* [out] */ IDirectory **dir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenFile( 
            /* [in] */ BSTR name,
            /* [out] */ IFile **file) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveFile( 
            /* [in] */ BSTR name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveDir( 
            /* [in] */ BSTR name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enumerate( 
            /* [out] */ IDirectoryEnumerator **direnum) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDirectoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirectory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirectory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirectory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IDirectory * This,
            /* [out] */ IDirectory **parent);
        
        HRESULT ( STDMETHODCALLTYPE *OpenDirectory )( 
            IDirectory * This,
            /* [in] */ BSTR name,
            /* [out] */ IDirectory **dir);
        
        HRESULT ( STDMETHODCALLTYPE *OpenFile )( 
            IDirectory * This,
            /* [in] */ BSTR name,
            /* [out] */ IFile **file);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFile )( 
            IDirectory * This,
            /* [in] */ BSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveDir )( 
            IDirectory * This,
            /* [in] */ BSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *Enumerate )( 
            IDirectory * This,
            /* [out] */ IDirectoryEnumerator **direnum);
        
        END_INTERFACE
    } IDirectoryVtbl;

    interface IDirectory
    {
        CONST_VTBL struct IDirectoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirectory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDirectory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDirectory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDirectory_GetParent(This,parent)	\
    ( (This)->lpVtbl -> GetParent(This,parent) ) 

#define IDirectory_OpenDirectory(This,name,dir)	\
    ( (This)->lpVtbl -> OpenDirectory(This,name,dir) ) 

#define IDirectory_OpenFile(This,name,file)	\
    ( (This)->lpVtbl -> OpenFile(This,name,file) ) 

#define IDirectory_RemoveFile(This,name)	\
    ( (This)->lpVtbl -> RemoveFile(This,name) ) 

#define IDirectory_RemoveDir(This,name)	\
    ( (This)->lpVtbl -> RemoveDir(This,name) ) 

#define IDirectory_Enumerate(This,direnum)	\
    ( (This)->lpVtbl -> Enumerate(This,direnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDirectory_INTERFACE_DEFINED__ */


#ifndef __IFile_INTERFACE_DEFINED__
#define __IFile_INTERFACE_DEFINED__

/* interface IFile */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("45bea892-5416-11e6-be20-08002766fdaa")
    IFile : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Read( 
            /* [size_is][in] */ BYTE *buff,
            /* [in] */ DWORD buffSize,
            /* [out] */ DWORD *read) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Write( 
            /* [size_is][in] */ BYTE *buff,
            DWORD buffSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ DWORD pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Tell( 
            /* [out] */ DWORD *pos) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ DWORD *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resize( 
            /* [in] */ DWORD newSize) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFile * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IFile * This,
            /* [size_is][in] */ BYTE *buff,
            /* [in] */ DWORD buffSize,
            /* [out] */ DWORD *read);
        
        HRESULT ( STDMETHODCALLTYPE *Write )( 
            IFile * This,
            /* [size_is][in] */ BYTE *buff,
            DWORD buffSize);
        
        HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IFile * This,
            /* [in] */ DWORD pos);
        
        HRESULT ( STDMETHODCALLTYPE *Tell )( 
            IFile * This,
            /* [out] */ DWORD *pos);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IFile * This,
            /* [out] */ DWORD *size);
        
        HRESULT ( STDMETHODCALLTYPE *Resize )( 
            IFile * This,
            /* [in] */ DWORD newSize);
        
        END_INTERFACE
    } IFileVtbl;

    interface IFile
    {
        CONST_VTBL struct IFileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFile_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFile_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFile_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFile_Read(This,buff,buffSize,read)	\
    ( (This)->lpVtbl -> Read(This,buff,buffSize,read) ) 

#define IFile_Write(This,buff,buffSize)	\
    ( (This)->lpVtbl -> Write(This,buff,buffSize) ) 

#define IFile_Seek(This,pos)	\
    ( (This)->lpVtbl -> Seek(This,pos) ) 

#define IFile_Tell(This,pos)	\
    ( (This)->lpVtbl -> Tell(This,pos) ) 

#define IFile_GetSize(This,size)	\
    ( (This)->lpVtbl -> GetSize(This,size) ) 

#define IFile_Resize(This,newSize)	\
    ( (This)->lpVtbl -> Resize(This,newSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFile_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


