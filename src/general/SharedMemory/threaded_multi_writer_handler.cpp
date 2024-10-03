#include "general/SharedMemory/threaded_multi_writer_handler.h"
#include <cstring>
#include <iostream>
#include "general/SharedMemory/bufferd_writer.h"
#include "general/SharedMemory/info.h"

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace SharedMemory {

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)

void ThreadedMultiWriterHandler::addWriter(
    SharedMemory::BufferedWriter writer) {
    if (m_initalized) { return; }
    m_writers.push_back(writer);
}

ThreadedMultiWriterHandler::~ThreadedMultiWriterHandler() {
    UnmapViewOfFile(m_memoryInfo);
    CloseHandle(m_infoDescriptor);
    CloseHandle(m_infoSem);
}

bool ThreadedMultiWriterHandler::initalize() {

    int size = sizeof(SharedMemory::ThreadedInfo);
    for (auto writer : m_writers) {
        size += sizeof(char) * (writer.InfoBufferName().length() + 1);
    }
    m_infoDescriptor =
        CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                          size, m_infoBufferName.c_str());

    if (m_infoDescriptor == nullptr) {
        std::cout << "Shared Memory Open of Name: " << m_infoBufferName
                  << " failed error: " << GetLastError() << std::endl;
        return false;
    }

    m_memoryInfo = (SharedMemory::ThreadedInfo*) MapViewOfFile(
        m_infoDescriptor, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (m_memoryInfo == nullptr) {
        std::cout << "Shared Memory Mapping of name: " << m_infoBufferName
                  << " failed error: " << GetLastError() << std::endl;
        return false;
    }

    m_memoryInfo->numberOfWriters = m_writers.size();
    m_memoryInfo->infoSize = size;

    std::string infoSemName = m_infoBufferName + "_sem";
    m_infoSem = CreateSemaphore(nullptr, 1, 1, infoSemName.c_str());
    if (m_infoSem == nullptr) {
        std::cout << "Initalization of Semaphore for shared memory of name: "
                  << m_infoBufferName << " failed errror: " << GetLastError()
                  << std::endl;
        return false;
    }

    char* infoMemory = (char*) (m_memoryInfo + 1);
    for (auto writer : m_writers) {
        std::strcpy(infoMemory, writer.InfoBufferName().c_str());
        infoMemory = (infoMemory + writer.InfoBufferName().length() + 1);
    }

    return true;
}
#else
void ThreadedMultiWriterHandler::addWriter(
    SharedMemory::BufferedWriter writer) {
    if (m_initalized) { return; }
    m_writers.push_back(writer);
}

bool ThreadedMultiWriterHandler::initalize() {

    int infoDescriptor =
        shm_open(m_infoBufferName.c_str(), O_CREAT | O_RDWR, 0666);

    if (infoDescriptor == -1) {
        std::cout << "Shared Memory Open of Name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    int size = sizeof(SharedMemory::ThreadedInfo);
    for (auto writer : m_writers) {
        size += sizeof(char) * (writer.InfoBufferName().length() + 1);
    }

    if (ftruncate(infoDescriptor, size) == -1) {
        std::cout << "Shared Memory Truncate of name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    m_memoryInfo = (SharedMemory::ThreadedInfo*) mmap(
        NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, infoDescriptor, 0);
    if (m_memoryInfo == MAP_FAILED) {
        std::cout << "Shared Memory Mapping of name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    m_memoryInfo->numberOfWriters = m_writers.size();
    m_memoryInfo->infoSize = size;

    if (sem_init(&m_memoryInfo->semaphore, 1, 1) == -1) {
        std::cout << "Initalization of Semaphore for shared memory of name: "
                  << m_infoBufferName << " failed errror: " << strerror(errno)
                  << std::endl;
        return false;
    }

    char* infoMemory = (char*) (m_memoryInfo + 1);
    for (auto writer : m_writers) {
        std::strcpy(infoMemory, writer.InfoBufferName().c_str());
        infoMemory = (infoMemory + writer.InfoBufferName().length() + 1);
    }

    return true;
}
#endif
}// namespace SharedMemory
