/****************************** Module Header ******************************\
Module Name:  FileContextMenuExt.cpp
Project:      taskCOMShellExtension

The context menu handler adds the menu item "Calculate the Sum"
to the context menu when you right click on the list of files (folders is
ignored) in the Windows Explorer. 
Clicking the menu item  writes information about selected files into the
log file.
\***************************************************************************/

#include "FileContextMenuExt.h"
#include "resource.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include <fstream>
#include <sys/stat.h>

#pragma comment(lib, "shlwapi.lib")


extern HINSTANCE g_hInst;
extern long g_cDllRef;

#define IDM_DISPLAY 0  // The command's identifier offset


const char * FileContextMenuExt::LogFile = "F:\\Cloud@mail.ru\\basecamp\\testTaskCOMShellExtension\\taskCOMShellExtension\\testLog.txt";

FileContextMenuExt::FileContextMenuExt(void) : m_cRef(1), 
    m_pszMenuText(L"&Calculate the Sum"),
    m_pszVerb("calculatesum"),
    m_pwszVerb(L"calculatesum"),
    m_pszVerbCanonicalName("CalculateTheSum"),
    m_pwszVerbCanonicalName(L"CalculateTheSum"),
    m_pszVerbHelpText("Calculate the Sum"),
    m_pwszVerbHelpText(L"Calculate the Sum")
{
    InterlockedIncrement(&g_cDllRef);


    // Load the bitmap for the menu item. 
    // If you want the menu item bitmap to be transparent, the color depth of 
    // the bitmap must not be greater than 8bpp.
    m_hMenuBmp = LoadImage(g_hInst, MAKEINTRESOURCE(IDB_SUM), 
        IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT);
}

FileContextMenuExt::~FileContextMenuExt(void)
{
    if (m_hMenuBmp)
    {
        DeleteObject(m_hMenuBmp);
        m_hMenuBmp = nullptr;
    }

    InterlockedDecrement(&g_cDllRef);
}


void FileContextMenuExt::WriteInformationToFile(HWND hWnd, const char * logFile)
{
    std::wofstream wfout;
    wfout.open(logFile);

    if (!wfout.is_open())
    {
        PWSTR wLogFile;
	    UINT count = MultiByteToWideChar(CP_ACP, 0, logFile, strlen(logFile), nullptr, 0);
	    if (count > 0)
	    {
    		wLogFile = SysAllocStringLen(0, count);
		    MultiByteToWideChar(CP_ACP, 0, logFile, strlen(logFile), wLogFile, count);
	    }
	    const PWSTR cantFile = L"Can\'t open file: ";
	    PWSTR message = SysAllocStringLen(0, wcslen(cantFile) + count + 1);
	    wcscpy(message, cantFile);
	    wcscat(message, wLogFile);
        MessageBox(hWnd, message, L"File write error", MB_OK);

	    SysFreeString(wLogFile);
        SysFreeString(message);
    } else {
        //m_listFiles.sort();
        m_fileList.sort([](const FileInfo & file1, const FileInfo & file2) {
            std::basic_string<wchar_t> first = file1.name;
            std::basic_string<wchar_t> second = file2.name;
            unsigned int i = 0;
            while ( (i < first.length()) && (i < second.length()) ) {
                if (tolower(first[i]) < tolower(second[i]))
                    return true;
                else if (tolower(first[i]) > tolower(second[i]))
                    return false;
                ++i;
            }
            return ( first.length() < second.length() );
        });

        // Write list of file names to log file.
        for (auto file : m_fileList)
        {
            wfout << file.name << L"\t\t\t\t" << file.size 
                  << L"\t\t" << file.ctime << std::endl;
        }
        wfout.close();
    }
}


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP FileContextMenuExt::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = 
    {
        QITABENT(FileContextMenuExt, IContextMenu),
        QITABENT(FileContextMenuExt, IShellExtInit), 
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FileContextMenuExt::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FileContextMenuExt::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

#pragma endregion


#pragma region IShellExtInit

// Initialize the context menu handler.
IFACEMETHODIMP FileContextMenuExt::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
    if (nullptr == pDataObj)
    {
        return E_INVALIDARG;
    }

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;


    // The pDataObj pointer contains the objects being acted upon. In this 
    // example, we get an HDROP handle for enumerating the selected files and 
    // folders.
    if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
    {
        // Get an HDROP handle.
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != nullptr)
        {
            // Determine how many files are involved in this operation.
            m_nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
            struct _stat st;

            for (UINT uFile = 0; uFile < m_nFiles; ++uFile)
            {
                // Get the path of the file.
                if (0 != DragQueryFile(hDrop, uFile, m_szSelectedFile, MAX_PATH))
                {
                    // Add to list only files but not directories.
                    if (_wstat(m_szSelectedFile, &st) == 0 && !(st.st_mode & _S_IFDIR)) {
                        //m_listFiles.push_back(PathFindFileName(m_szSelectedFile));
                        m_fileInfo.name = PathFindFileName(m_szSelectedFile);
                        m_fileInfo.size = st.st_size;
                        m_fileInfo.ctime = st.st_ctime;
                        m_fileList.push_back(m_fileInfo);
                    }
                }
            }
            
            GlobalUnlock(stm.hGlobal);
        }

        ReleaseStgMedium(&stm);
    }

    // If any value other than S_OK is returned from the method, the context 
    // menu item is not displayed.
    return m_fileList.size() > 0 ? S_OK : E_FAIL;
}

#pragma endregion


#pragma region IContextMenu

//
//   FUNCTION: FileContextMenuExt::QueryContextMenu
//
//   PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
//            context menu handler to add its menu items to the menu. It 
//            passes in the HMENU handle in the hmenu parameter. The 
//            indexMenu parameter is set to the index to be used for the 
//            first menu item that is to be added.
//
IFACEMETHODIMP FileContextMenuExt::QueryContextMenu(
    HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    // If uFlags include CMF_DEFAULTONLY then we should not do anything.
    if (CMF_DEFAULTONLY & uFlags)
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

    // Use either InsertMenu or InsertMenuItem to add menu items.
    // Learn how to add sub-menu from:
    // http://www.codeproject.com/KB/shell/ctxextsubmenu.aspx

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
    mii.wID = idCmdFirst + IDM_DISPLAY;
    mii.fType = MFT_STRING;
    mii.dwTypeData = m_pszMenuText;
    mii.fState = MFS_ENABLED;
    mii.hbmpItem = static_cast<HBITMAP>(m_hMenuBmp);
    if (!InsertMenuItem(hMenu, indexMenu, TRUE, &mii))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Add a separator.
    MENUITEMINFO sep = { sizeof(sep) };
    sep.fMask = MIIM_TYPE;
    sep.fType = MFT_SEPARATOR;
    if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Return an HRESULT value with the severity set to SEVERITY_SUCCESS. 
    // Set the code value to the offset of the largest command identifier 
    // that was assigned, plus one (1).
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_DISPLAY + 1));
}


//
//   FUNCTION: FileContextMenuExt::InvokeCommand
//
//   PURPOSE: This method is called when a user clicks a menu item to tell 
//            the handler to run the associated command. The lpcmi parameter 
//            points to a structure that contains the needed information.
//
IFACEMETHODIMP FileContextMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    BOOL fUnicode = FALSE;

    // Determine which structure is being passed in, CMINVOKECOMMANDINFO or 
    // CMINVOKECOMMANDINFOEX based on the cbSize member of lpcmi. Although 
    // the lpcmi parameter is declared in Shlobj.h as a CMINVOKECOMMANDINFO 
    // structure, in practice it often points to a CMINVOKECOMMANDINFOEX 
    // structure. This struct is an extended version of CMINVOKECOMMANDINFO 
    // and has additional members that allow Unicode strings to be passed.
    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    {
        if (pici->fMask & CMIC_MASK_UNICODE)
        {
            fUnicode = TRUE;
        }
    }

    // Determines whether the command is identified by its offset or verb.
    // There are two ways to identify commands:
    // 
    //   1) The command's verb string 
    //   2) The command's identifier offset
    // 
    // If the high-order word of lpcmi->lpVerb (for the ANSI case) or 
    // lpcmi->lpVerbW (for the Unicode case) is nonzero, lpVerb or lpVerbW 
    // holds a verb string. If the high-order word is zero, the command 
    // offset is in the low-order word of lpcmi->lpVerb.

    // For the ANSI case, if the high-order word is not zero, the command's 
    // verb string is in lpcmi->lpVerb. 
    if (!fUnicode && HIWORD(pici->lpVerb))
    {
        // Is the verb supported by this context menu extension?
        if (StrCmpIA(pici->lpVerb, m_pszVerb) == 0)
        {
            WriteInformationToFile(pici->hwnd, LogFile);
        }
        else
        {
            // If the verb is not recognized by the context menu handler, it 
            // must return E_FAIL to allow it to be passed on to the other 
            // context menu handlers that might implement that verb.
            return E_FAIL;
        }
    }

    // For the Unicode case, if the high-order word is not zero, the 
    // command's verb string is in lpcmi->lpVerbW. 
    else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW))
    {
        // Is the verb supported by this context menu extension?
        if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, m_pwszVerb) == 0)
        {
            WriteInformationToFile(pici->hwnd, LogFile);
        }
        else
        {
            // If the verb is not recognized by the context menu handler, it 
            // must return E_FAIL to allow it to be passed on to the other 
            // context menu handlers that might implement that verb.
            return E_FAIL;
        }
    }

    // If the command cannot be identified through the verb string, then 
    // check the identifier offset.
    else
    {
        // Is the command identifier offset supported by this context menu 
        // extension?
        if (LOWORD(pici->lpVerb) == IDM_DISPLAY)
        {
            WriteInformationToFile(pici->hwnd, LogFile);
        }
        else
        {
            // If the verb is not recognized by the context menu handler, it 
            // must return E_FAIL to allow it to be passed on to the other 
            // context menu handlers that might implement that verb.
            return E_FAIL;
        }
    }

    return S_OK;
}


//
//   FUNCTION: CFileContextMenuExt::GetCommandString
//
//   PURPOSE: If a user highlights one of the items added by a context menu 
//            handler, the handler's IContextMenu::GetCommandString method is 
//            called to request a Help text string that will be displayed on 
//            the Windows Explorer status bar. This method can also be called 
//            to request the verb string that is assigned to a command. 
//            Either ANSI or Unicode verb strings can be requested. This 
//            example only implements support for the Unicode values of 
//            uFlags, because only those have been used in Windows Explorer 
//            since Windows 2000.
//
IFACEMETHODIMP FileContextMenuExt::GetCommandString(UINT_PTR idCommand, 
    UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
    HRESULT hr = E_INVALIDARG;

    if (idCommand == IDM_DISPLAY)
    {
        switch (uFlags)
        {
        case GCS_HELPTEXTW:
            // Only useful for pre-Vista versions of Windows that have a 
            // Status bar.
            hr = StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, 
                m_pwszVerbHelpText);
            break;

        case GCS_VERBW:
            // GCS_VERBW is an optional feature that enables a caller to 
            // discover the canonical name for the verb passed in through 
            // idCommand.
            hr = StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, 
                m_pwszVerbCanonicalName);
            break;

        default:
            hr = S_OK;
        }
    }

    // If the command (idCommand) is not supported by this context menu 
    // extension handler, return E_INVALIDARG.

    return hr;
}

#pragma endregion