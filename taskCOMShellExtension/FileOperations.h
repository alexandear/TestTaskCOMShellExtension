/****************************** Module Header ******************************\
Module Name:  FileOperations.h
Project:      taskCOMShellExtension

The file declares the file operations such as calculate per byte sum of file
and write short information to log file. 
\***************************************************************************/

#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <boost\thread.hpp>
#include <boost\thread\mutex.hpp>
#include <ctime>
#include <queue>
#include <string>
#include <set>
#include <vector>
#include <Windows.h>

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
    FileOperations(const wstring & logFilePath);
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
    void appendLogFile(const wstring & fileInfoStr);

    unsigned m_maxThreads;
    vector<boost::thread> m_threadList;
	boost::mutex m_mtxRead, m_mtxWrite;
    wstring m_logFilePath;
};

#endif // FILEOPERATIONS_H
