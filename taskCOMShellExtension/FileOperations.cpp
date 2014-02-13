/****************************** Module Header ******************************\
Module Name:  FileOperations.cpp
Project:      taskCOMShellExtension

The file declares the file operations such as calculate per byte sum of file
and write short information to log file. 
\***************************************************************************/
#include "stdafx.h"
#include "FileOperations.h"


FileOperations::FileOperations() {

    wfout.imbue(std::locale(".1251"));

    std::srand(std::time(0));
    int nLogFile = rand() % 2014 + 1;
    wchar_t buf[4];
    swprintf(buf, 4, L"%u", nLogFile);
    m_logFilePath.append(L"FileInfo").append(buf).append(L".log");

    m_maxThreads = boost::thread::hardware_concurrency();
    if (m_maxThreads == 0) m_maxThreads = 2;
}


void FileOperations::addFile(const wstring & filePath) {
    m_fileSet.insert(filePath);
    m_perByteSumQueue.push(filePath);
}


void FileOperations::run() {
    // Launch thread pool.
    size_t sizeList = m_fileSet.size();

    wfout.open(m_logFilePath, std::ios_base::out | std::ios_base::app);

    UINT threads = sizeList < m_maxThreads ? sizeList : m_maxThreads;

    for (UINT i = 0; i < threads; ++i) {
        m_threadList.push_back(boost::thread(&FileOperations::getInfoAllFiles, this, &m_perByteSumQueue));
    }
    
    for (UINT i = 0; i < threads; ++i){
        m_threadList[i].join();
    }
    
    wfout.close();
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


void FileOperations::getInfoAllFiles(std::queue<wstring> * perByteSumQueue) {
    static boost::mutex mtxRead, mtxWrite;
    while(!perByteSumQueue->empty()) {
        mtxRead.lock();
            wstring filePath = perByteSumQueue->front();
            perByteSumQueue->pop();
        mtxRead.unlock();

        FileInfo fileInfo = getInfo(filePath, fileInfo);

        mtxWrite.lock();
            wstring fileName(PathFindFileName(filePath.c_str()));
            if (wfout.is_open()) {
                wfout << getFileInfoStr(fileName, fileInfo) << std::endl;
            }
        mtxWrite.unlock();
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


int FileOperations::numDigits(int number) {
    int digits = 0;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}
