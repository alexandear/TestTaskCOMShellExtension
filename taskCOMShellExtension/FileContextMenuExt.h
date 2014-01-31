/****************************** Module Header ******************************\
Module Name:  FileContextMenuExt.h
Project:      taskCOMShellExtension

The context menu handler adds the menu item "Calculate the Sum"
to the context menu when you right click on the list of files (folders is
ignored) in the Windows Explorer. 
Clicking the menu item  writes information about selected files into the
log file.
\***************************************************************************/

#pragma once

#include <windows.h>
#include <shlobj.h>     // For IShellExtInit and IContextMenu
#include <string>
#include <vector>
#include <list>
#include <ctime>

class FileContextMenuExt : public IShellExtInit, public IContextMenu
{
public:
    typedef std::list<std::basic_string<wchar_t> > stringList;

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

    static const char * LogFile;

protected:
    ~FileContextMenuExt(void);

private:
    // Reference count of component.
    long m_cRef;

    // The name of the selected file.
    wchar_t m_szSelectedFile[MAX_PATH];

    // Number of the selected files.
    UINT m_nFiles;

    // Names of the selected files.
    stringList m_listFiles;
    
    // Information about file.
    struct FileInfo
    {
        std::basic_string<wchar_t> name;
        off_t size;
        std::time_t ctime;
    } m_fileInfo;

    std::list<FileInfo> m_fileList;

    void WriteInformationToFile(HWND hWnd, const char * logFile);

    PWSTR m_pszMenuText;
    HANDLE m_hMenuBmp;
    PCSTR m_pszVerb;
    PCWSTR m_pwszVerb;
    PCSTR m_pszVerbCanonicalName;
    PCWSTR m_pwszVerbCanonicalName;
    PCSTR m_pszVerbHelpText;
    PCWSTR m_pwszVerbHelpText;
};