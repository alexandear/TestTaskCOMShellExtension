/****************************** Module Header ******************************\
Module Name:  FileOperations.cpp
Project:      taskCOMShellExtension

The file declares the file operations such as calculate per byte sum of file
and write short information to log file. 
\***************************************************************************/

#include <fstream>
#include <limits>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <sys/stat.h>
#include "FileOperations.h"

FileOperations::FileOperations() : m_logFilePath(L"FilesInfo.log") {
    init();
}

FileOperations::FileOperations(const wstring & logFilePath) : m_logFilePath(logFilePath) {
    init();
}

void FileOperations::init() {
    m_maxThreads = boost::thread::hardware_concurrency();
	if(!m_maxThreads)
		m_maxThreads = 2;
}

FileOperations::~FileOperations() { }

void FileOperations::addFile(const wstring & filePath) {
    m_fileSet.insert(filePath);
    m_queuePerByteSum.push(filePath);
}

void FileOperations::run() {
    // Launch thread pool.
    size_t listSize = m_fileSet.size();
    UINT threads = listSize < m_maxThreads ? listSize : m_maxThreads;
	for (UINT i = 0; i < threads; ++i) {
        m_threadList.push_back(boost::thread(&FileOperations::getInfoAllFiles, this, &m_queuePerByteSum));
	}
    
    for (UINT i = 0; i < threads; ++i){
		m_threadList[i].join();
	}
}

void FileOperations::appendLogFile(const wstring & fileInfoStr) {
    m_filesInfoList.clear();
    m_filesInfoList.push_back(fileInfoStr);

    std::wfstream wfstr;
    wfstr.open(m_logFilePath, std::ios_base::in);
    
    wstring fileInfoLine;
    if (wfstr.is_open()) {
        do {
            std::getline(wfstr, fileInfoLine);
            if(fileInfoLine != L"") {
                m_filesInfoList.push_back(fileInfoLine);
            }
        } while (wfstr);
        m_filesInfoList.sort();
        wfstr.close();
    }

    wfstr.open(m_logFilePath, std::ios_base::out);
    if (wfstr.is_open()) {
        for (auto file: m_filesInfoList) {
            wfstr << file << std::endl;
        }
        wfstr.close();
    }
}

FileOperations::FileInfo & FileOperations::getInfo(const wstring & filePath, FileInfo & fileInfo) {
    struct _stat st;
    if (_wstat(filePath.c_str(), &st) == 0) {
        fileInfo.size = st.st_size;
        fileInfo.creationTime = st.st_ctime;
        fileInfo.perByteSum = calculatePerByteSum(filePath);
    }
    return fileInfo;
}

wstring FileOperations::getFileInfoStr(const wstring & fileName, const FileInfo & fileInfo) {
    wchar_t timeBuf[26];
    _wctime_s(timeBuf, 26, &fileInfo.creationTime);
    timeBuf[24] = '\0';

    const int off_t_digits = std::numeric_limits<off_t>::digits;
    wchar_t sizeBuf[off_t_digits];
    swprintf(sizeBuf, off_t_digits, L"%i", fileInfo.size);

    const int DWORD_digits = std::numeric_limits<DWORD>::digits;
    wchar_t perByteBuf[DWORD_digits];
    swprintf(perByteBuf, DWORD_digits, L"%u", fileInfo.perByteSum);

    wstring fileInfoStr;
    fileInfoStr.append(fileName + L"    ").append(sizeBuf).append(L" bytes    ")
        .append(timeBuf + 4).append(L"    ").append(perByteBuf);
    return fileInfoStr;
}

void FileOperations::getInfoAllFiles(std::queue<wstring> * queuePerByteSum) {
    while(!queuePerByteSum->empty()) {
        m_mtxRead.lock();
			wstring filePath = queuePerByteSum->front();
			queuePerByteSum->pop();
		m_mtxRead.unlock();

        FileInfo fileInfo = getInfo(filePath, fileInfo);

        m_mtxWrite.lock();
            wstring fileName(PathFindFileName(filePath.c_str()));
            appendLogFile(getFileInfoStr(fileName, fileInfo));
		m_mtxWrite.unlock();
    }
}

DWORD FileOperations::calculatePerByteSum(const wstring & filePath) {
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
