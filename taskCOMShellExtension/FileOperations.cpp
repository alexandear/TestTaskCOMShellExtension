#include <fstream>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <sys/stat.h>
#include "FileOperations.h"

FileOperations::FileOperations() { }

FileOperations::~FileOperations() { }

void FileOperations::addFile(const std::wstring & filePath) {
    std::wstring fileName(PathFindFileName(filePath.c_str()));
    m_fileMap.insert ( FileInfoPair(fileName, getInfo(filePath, m_fileInfo)) );
}

HRESULT FileOperations::writeLogFile(const std::wstring & logFile) {
    using std::wofstream;
    wofstream wfout;
    wfout.open(logFile, wofstream::app | wofstream::out);
    if (wfout.is_open()) {
        char timebuf[26];
        for (auto file : m_fileMap) {
            ctime_s(timebuf, 26, &file.second.creationTime);
            timebuf[24] = '\0';
            wfout << file.first << L"   " << file.second.size << L" bytes   " << (timebuf + 4)
                  << L"   " << file.second.perByteSum << std::endl;
        }
        wfout.close();
        return S_OK;
    } else {
        return E_FAIL;
    }
}


FileOperations::FileInfo & FileOperations::getInfo(const std::wstring & filePath, FileInfo & fileInfo) {
    struct _stat st;
    if (_wstat(filePath.c_str(), &st) == 0) {
        fileInfo.size = st.st_size;
        fileInfo.creationTime = st.st_ctime;
        fileInfo.perByteSum = calculatePerByteSum(filePath);
    }
    return fileInfo;
}

DWORD FileOperations::calculatePerByteSum(const std::wstring & filePath) {
    DWORD checksum = 0;
    std::ifstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        char byte;
        while (!file.eof()) {
            file.read(&byte, sizeof(char));
            checksum += byte;
        }
        file.close();
    }

    return checksum;
}