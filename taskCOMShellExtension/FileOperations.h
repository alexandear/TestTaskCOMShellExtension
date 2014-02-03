/****************************** Module Header ******************************\
Module Name:  FileOperations.h
Project:      taskCOMShellExtension

The file declares the file operations such as calculate per byte sum and
write log file. 
\***************************************************************************/

#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <map>
#include <string>
#include <ctime>
#include <Windows.h>


class FileOperations {
    struct FileInfo;
    typedef std::pair<std::wstring, FileInfo> FileInfoPair;
public:
    FileOperations();
    ~FileOperations();
    void addFile(const std::wstring & filePath);
    HRESULT writeLogFile(const std::wstring & logFile);
private:
    FileInfo & getInfo(const std::wstring & fileName, FileInfo & fileInfo);
    struct FileInfo {
        off_t size;
        std::time_t creationTime;
        DWORD perByteSum;
    } m_fileInfo;

    std::map<std::wstring, FileInfo> m_fileMap;
};

#endif // FILEOPERATIONS_H