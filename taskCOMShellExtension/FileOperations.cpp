#include <fstream>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <sys/stat.h>
#include "FileOperations.h"

FileOperations::FileOperations() {

}

FileOperations::~FileOperations() {
}

void FileOperations::addFile(const std::wstring & filePath) {
    std::wstring fileName(PathFindFileName(filePath.c_str()));
    m_fileMap.insert ( FileInfoPair(fileName, getInfo(fileName, m_fileInfo)) );
}

HRESULT FileOperations::writeLogFile(const std::wstring & logFile) {
    using std::wofstream;
    wofstream wfout;
    wfout.open(logFile, wofstream::app | wofstream::out);
    if (wfout.is_open()) {
        char timebuf[26];
        for (auto file : m_fileMap)
        {
            ctime_s(timebuf, 26, &file.second.creationTime);
            wfout << file.first << L"   " << file.second.size << L" bytes   " << (timebuf + 4);
        }
        wfout.close();
        return S_OK;
    } else {
        return E_FAIL;
    }
}


FileOperations::FileInfo & FileOperations::getInfo(const std::wstring & fileName, FileInfo & fileInfo) {
    struct _stat st;
    if (_wstat(fileName.c_str(), &st) == 0) {
        fileInfo.size = st.st_size;
        fileInfo.creationTime = st.st_ctime;
    }
    return fileInfo;
}