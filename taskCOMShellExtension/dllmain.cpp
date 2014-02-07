/****************************** Module Header ******************************\
Module Name:  dllmain.cpp
Project:      taskCOMShellExtension

The file implements DllMain, and the DllGetClassObject, DllCanUnloadNow, 
DllRegisterServer, DllUnregisterServer functions that are necessary for a COM 
DLL. 

DllGetClassObject invokes the class factory defined in ClassFactory.h/cpp and 
queries to the specific interface.

DllCanUnloadNow checks if we can unload the component from the memory.

DllRegisterServer registers the COM server and the context menu handler in 
the registry by invoking the helper functions defined in Reg.h/cpp. The 
context menu handler is associated with the .cpp file class.

DllUnregisterServer unregisters the COM server and the context menu handler. 
\***************************************************************************/
#include "stdafx.h"
#include "ClassFactory.h"
#include "Reg.h"


// {FE432728-EFD4-401D-8E41-4E7A66A85358}
const CLSID CLSID_FileContextMenuExt = 
{ 0xfe432728, 0xefd4, 0x401d, { 0x8e, 0x41, 0x4e, 0x7a, 0x66, 0xa8, 0x53, 0x58 } };



HINSTANCE   g_hInst     = nullptr;
long        g_cDllRef   = 0;

const wchar_t * pszFileType = L"*";


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        // Hold the instance of this DLL module, we will use it to get the 
        // path of the DLL to register the component.
        g_hInst = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


//
//   FUNCTION: DllGetClassObject
//
//   PURPOSE: Create the class factory and query to the specific interface.
//
//   PARAMETERS:
//   * rclsid - The CLSID that will associate the correct data and code.
//   * riid - A reference to the identifier of the interface that the caller 
//     is to use to communicate with the class object.
//   * ppv - The address of a pointer variable that receives the interface 
//     pointer requested in riid. Upon successful return, *ppv contains the 
//     requested interface pointer. If an error occurs, the interface pointer 
//     is NULL. 
//
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

    if (IsEqualCLSID(CLSID_FileContextMenuExt, rclsid)) {
        hr = E_OUTOFMEMORY;

        ClassFactory *pClassFactory = new ClassFactory();
        if (pClassFactory) {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}


//
//   FUNCTION: DllCanUnloadNow
//
//   PURPOSE: Check if we can unload the component from the memory.
//
//   NOTE: The component can be unloaded from the memory when its reference 
//   count is zero (i.e. nobody is still using the component).
// 
STDAPI DllCanUnloadNow(void) {
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}


//
//   FUNCTION: DllRegisterServer
//
//   PURPOSE: Register the COM server and the context menu handler.
// 
STDAPI DllRegisterServer(void) {
    HRESULT hr;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Register the component.
    hr = RegisterInprocServer(szModule, CLSID_FileContextMenuExt, 
        L"CppShellExtContextMenuHandler.FileContextMenuExt Class", 
        L"Apartment");
    if (SUCCEEDED(hr)) {
        // Register the context menu handler. The context menu handler is 
        // associated with the .cpp file class.
        hr = RegisterShellExtContextMenuHandler(pszFileType, 
            CLSID_FileContextMenuExt, 
            L"CppShellExtContextMenuHandler.FileContextMenuExt");
    }

    return hr;
}


//
//   FUNCTION: DllUnregisterServer
//
//   PURPOSE: Unregister the COM server and the context menu handler.
// 
STDAPI DllUnregisterServer(void) {
    HRESULT hr = S_OK;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Unregister the component.
    hr = UnregisterInprocServer(CLSID_FileContextMenuExt);
    if (SUCCEEDED(hr)) {
        // Unregister the context menu handler.
        hr = UnregisterShellExtContextMenuHandler(pszFileType, 
            CLSID_FileContextMenuExt);
    }

    return hr;
}