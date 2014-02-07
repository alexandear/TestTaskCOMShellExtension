/****************************** Module Header ******************************\
Module Name:  FileOperations.h
Project:      taskCOMShellExtension

The file declares the file operations such as calculate per byte sum of file
and write short information to log file. 
\***************************************************************************/

#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include "stdafx.h"

using std::list;
using std::pair;
using std::queue;
using std::set;
using std::vector;
using std::wstring;


class FileOperations {
    struct FileInfo {
        off_t size;
        std::time_t creationTime;
        DWORD perByteSum;
    } m_fileInfo;
    typedef pair<wstring, FileInfo> FileInfoPair;
public:
    FileOperations();
    ~FileOperations();
    void addFile(const wstring & filePath);
    void run();
private:
    set<wstring> m_fileSet;
    queue<wstring> m_queuePerByteSum;
    list<wstring> m_filesInfoList;

    void init();
    FileInfo & getInfo(const wstring & filePath, FileInfo & fileInfo);
    wstring getFileInfoStr(const wstring & fileName, const FileInfo & fileInfo);
    DWORD calculatePerByteSum(const wstring & filePath);
    void getInfoAllFiles(queue<wstring> * queuePerByteSum);

    unsigned m_maxThreads;
    vector<boost::thread> m_threadList;
    boost::mutex m_mtxRead, m_mtxWrite;

    std::wofstream wfout;
    wstring m_logFilePath;
    unsigned m_nLogFile;

    int numDigits(int number);
};

#endif // FILEOPERATIONS_H
