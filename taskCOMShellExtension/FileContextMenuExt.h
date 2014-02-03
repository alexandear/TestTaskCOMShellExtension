/****************************** Module Header ******************************\
Module Name:  FileContextMenuExt.h
Project:      taskCOMShellExtension

The context menu handler adds the menu item "Calculate the Sum"
to the context menu when you right click on the list of files (folders is
ignored) in the Windows Explorer. 
Clicking the menu item  writes information about selected files into the
log file.
\***************************************************************************/

#ifndef FILECONTEXTMENUEXT_H
#define FILECONTEXTMENUEXT_H

#include <windows.h>
#include <shlobj.h>     // For IShellExtInit and IContextMenu
#include <string>
#include <list>
#include <ctime>
#include "FileOperations.h"

class FileContextMenuExt : public IShellExtInit, public IContextMenu {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);
	
    FileContextMenuExt(void);

    static const std::wstring LogFile;

protected:
    ~FileContextMenuExt(void);

private:
    // Reference count of component.
    long m_cRef;

    // The name of the selected file.
    wchar_t m_szSelectedFile[MAX_PATH];

    FileOperations fileProcessor;

    PWSTR m_pszMenuText;
    HANDLE m_hMenuBmp;
    PCSTR m_pszVerb;
    PCWSTR m_pwszVerb;
    PCSTR m_pszVerbCanonicalName;
    PCWSTR m_pwszVerbCanonicalName;
    PCSTR m_pszVerbHelpText;
    PCWSTR m_pwszVerbHelpText;
};

#endif // FILECONTEXTMENUEXT_H
