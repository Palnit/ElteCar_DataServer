#include "general/SharedMemory/bufferd_writer.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include "general/SharedMemory/info.h"

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#endif
namespace SharedMemory {

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)
BufferedWriter::BufferedWriter(const std::string& infoBufferName,
                               const std::string& defaultBufferName,
                               int bufferCount)
    : m_memoryInfo(nullptr),
      m_infoBufferName(infoBufferName),
      m_size(2000000),
      m_infoSize(sizeof(SharedMemory::Info)),
      m_initalized(false) {
    for (int i = 1; i <= bufferCount; i++) {
        std::string name = defaultBufferName + std::to_string(i);
        std::string sem_name = name + "_sem";
        m_infoSize +=
            sizeof(char) * (name.length() + 1) + (sem_name.length() + 1);
        m_memoryBuffer.push_back({name, sem_name, nullptr, nullptr, nullptr});
    }
}

BufferedWriter::BufferedWriter(const std::string& infoBufferName,
                               int size,
                               const std::string& defaultBufferName,
                               int bufferCount)
    : m_memoryInfo(nullptr),
      m_infoBufferName(infoBufferName),
      m_size(size),
      m_infoSize(sizeof(SharedMemory::Info)),
      m_initalized(false) {
    for (int i = 1; i <= bufferCount; i++) {
        std::string name = defaultBufferName + std::to_string(i);
        std::string sem_name = name + "_sem";
        m_infoSize +=
            sizeof(char) * (name.length() + 1) + (sem_name.length() + 1);
        m_memoryBuffer.push_back({name, sem_name, nullptr, nullptr, nullptr});
    }
}

bool BufferedWriter::initalize() {
    m_infoDescriptor =
        CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                          m_infoSize, m_infoBufferName.c_str());

    if (m_infoDescriptor == nullptr) {
        std::cout << "Shared Memory Open of Name: " << m_infoBufferName
                  << " failed error: " << GetLastError() << std::endl;
        return false;
    }

    m_memoryInfo = (SharedMemory::Info*) MapViewOfFile(
        m_infoDescriptor, FILE_MAP_ALL_ACCESS, 0, 0, m_infoSize);
    if (m_memoryInfo == nullptr) {
        std::cout << "Shared Memory Mapping of name: " << m_infoBufferName
                  << " failed error: " << GetLastError() << std::endl;
        return false;
    }

    m_memoryInfo->bufferNumber = 0;
    m_memoryInfo->infoBufferSize = m_infoSize;
    m_memoryInfo->bufferSize = m_size;
    m_memoryInfo->bufferNamesCount = m_memoryBuffer.size();
    std::string infoSemName = m_infoBufferName + "_sem";
    m_infoSem = CreateSemaphore(nullptr, 1, 1, infoSemName.c_str());
    if (m_infoSem == nullptr) {
        std::cout << "Initalization of Semaphore for shared memory of name: "
                  << m_infoBufferName << " failed errror: " << GetLastError()
                  << std::endl;
        return false;
    }

    char* infoMemory = (char*) (m_memoryInfo + 1);
    for (auto& [name, sem_name, descript, pointer, semaphore] :
         m_memoryBuffer) {
        std::strcpy(infoMemory, name.c_str());
        infoMemory = (char*) (infoMemory + name.length() + 1);

        semaphore = CreateSemaphore(nullptr, 1, 1, sem_name.c_str());
        if (semaphore == nullptr) {
            std::cout
                << "Initalization of Semaphore for shared memory of name: "
                << sem_name << " failed errror: " << GetLastError()
                << std::endl;
            return false;
        }

        descript = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr,
                                     PAGE_READWRITE, 0, m_size, name.c_str());

        if (descript == nullptr) {
            std::cout << "Shared Memory Open of Name: " << name
                      << " failed error: " << GetLastError() << std::endl;
            return false;
        }

        pointer = MapViewOfFile(descript, FILE_MAP_ALL_ACCESS, 0, 0, m_size);
        if (pointer == nullptr) {
            std::cout << "Shared Memory Mapping of name: " << name
                      << " failed error: " << GetLastError() << std::endl;
            return false;
        }
    }
    m_initalized = true;
    return true;
}

BufferedWriter::~BufferedWriter() {
    UnmapViewOfFile(m_memoryInfo);
    CloseHandle(m_infoDescriptor);
    CloseHandle(m_infoSem);
    for (auto& [name, sem_name, descript, pointer, sem] : m_memoryBuffer) {
        UnmapViewOfFile(pointer);
        CloseHandle(descript);
        CloseHandle(sem);
    }
}

bool BufferedWriter::writeMemory(const void* memory, std::size_t size) {
    if (!m_initalized) {
        if ((m_initalized = initalize()) == false) { return false; }
    }
    switch (WaitForSingleObject(m_infoSem, 10)) {
        case WAIT_ABANDONED:
            std::cout << "Semaphore wait Abandoned: " << std::endl;
            return false;
        case WAIT_TIMEOUT:
            std::cout << "Semaphore wait Timeout: " << std::endl;
            return false;
        case WAIT_FAILED:
            std::cout << "Semaphore wait Failed: " << std::endl;
            return false;
    }
    m_memoryInfo->bufferNumber++;
    if (m_memoryInfo->bufferNumber == m_memoryInfo->bufferNamesCount) {
        m_memoryInfo->bufferNumber = 0;
    }
    auto& [name, sem_name, descript, pointer, sem] =
        m_memoryBuffer[m_memoryInfo->bufferNumber];

    if (size > m_size) {
        //m_size = size * 2;
        throw new std::logic_error("Not Implemented");
    }

    switch (WaitForSingleObject(sem, 10)) {
        case WAIT_ABANDONED:
            std::cout << "Semaphore wait Abandoned: " << std::endl;
            return false;
        case WAIT_TIMEOUT:
            std::cout << "Semaphore wait Timeout: " << std::endl;
            return false;
        case WAIT_FAILED:
            std::cout << "Semaphore wait Fail: " << std::endl;
            return false;
    }
    std::memcpy(pointer, memory, size);
    if (m_memoryInfo->bufferSize != m_size) {
        m_memoryInfo->bufferSize = m_size;
    }
    m_memoryInfo->dataSize = size;
    if (!ReleaseSemaphore(sem, 1, nullptr)) {
        std::cout << "Semaphore post error: " << GetLastError();
        return false;
    }
    if (!ReleaseSemaphore(m_infoSem, 1, nullptr)) {
        std::cout << "Semaphore post error: " << GetLastError();
        return false;
    }
    return true;
};
#else
BufferedWriter::BufferedWriter(const std::string& infoBufferName,
                               const std::string& defaultBufferName,
                               int bufferCount)
    : m_memoryInfo(nullptr),
      m_infoBufferName(infoBufferName),
      m_size(2000000),
      m_infoSize(sizeof(SharedMemory::Info)),
      m_initalized(false) {
    for (int i = 1; i <= bufferCount; i++) {
        std::string name = defaultBufferName + std::to_string(i);
        m_infoSize += sizeof(char) * (name.length() + 1) + sizeof(sem_t);
        m_memoryBuffer.push_back({name, -1, nullptr, nullptr});
    }
}

BufferedWriter::BufferedWriter(const std::string& infoBufferName,
                               int size,
                               const std::string& defaultBufferName,
                               int bufferCount)
    : m_memoryInfo(nullptr),
      m_infoBufferName(infoBufferName),
      m_size(size),
      m_infoSize(sizeof(SharedMemory::Info)),
      m_initalized(false) {
    for (int i = 1; i <= bufferCount; i++) {
        std::string name = defaultBufferName + std::to_string(i);
        m_infoSize += sizeof(char) * (name.length() + 1) + sizeof(sem_t);
        m_memoryBuffer.push_back({name, -1, nullptr, nullptr});
    }
}

bool BufferedWriter::initalize() {
    int infoDescriptor =
        shm_open(m_infoBufferName.c_str(), O_CREAT | O_RDWR, 0666);

    if (infoDescriptor == -1) {
        std::cout << "Shared Memory Open of Name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    if (ftruncate(infoDescriptor, m_infoSize) == -1) {
        std::cout << "Shared Memory Truncate of name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    m_memoryInfo =
        (SharedMemory::Info*) mmap(NULL, m_infoSize, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, infoDescriptor, 0);
    if (m_memoryInfo == MAP_FAILED) {
        std::cout << "Shared Memory Mapping of name: " << m_infoBufferName
                  << " failed error: " << strerror(errno) << std::endl;
        return false;
    }

    m_memoryInfo->bufferNumber = 0;
    m_memoryInfo->infoBufferSize = m_infoSize;
    m_memoryInfo->bufferSize = m_size;
    m_memoryInfo->bufferNamesCount = m_memoryBuffer.size();
    if (sem_init(&(m_memoryInfo->semaphore), 1, 1) == -1) {
        std::cout << "Initalization of Semaphore for shared memory of name: "
                  << m_infoBufferName << " failed errror: " << strerror(errno)
                  << std::endl;
        return false;
    }

    char* infoMemory = (char*) (m_memoryInfo + 1);
    for (auto& [name, descript, pointer, semaphore] : m_memoryBuffer) {
        std::strcpy(infoMemory, name.c_str());
        semaphore = (sem_t*) (infoMemory + name.length() + 1);
        infoMemory = (char*) (semaphore + 1);

        if (sem_init(semaphore, 1, 1) == -1) {
            std::cout
                << "Initalization of Semaphore for shared memory of name: "
                << name << " failed errror: " << strerror(errno) << std::endl;
            return false;
        }

        descript = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);

        if (descript == -1) {
            std::cout << "Shared Memory Open of Name: " << name
                      << " failed error: " << strerror(errno) << std::endl;
            return false;
        }

        if (ftruncate(descript, m_size) == -1) {
            std::cout << "Shared Memory Truncate of name: " << name
                      << " failed error: " << strerror(errno) << std::endl;
            return false;
        }

        pointer =
            mmap(NULL, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, descript, 0);

        if (pointer == MAP_FAILED) {
            std::cout << "Shared Memory Mapping of name: " << name
                      << " failed error: " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

BufferedWriter::~BufferedWriter() {
    shm_unlink(m_infoBufferName.c_str());
    for (auto& [name, descript, pointer, sem] : m_memoryBuffer) {
        shm_unlink(name.c_str());
    }
}

bool BufferedWriter::writeMemory(const void* memory, std::size_t size) {
    if (!m_initalized) {
        if ((m_initalized = initalize()) == false) { return false; }
    }
    if (sem_wait(&m_memoryInfo->semaphore) == -1) {
        std::cout << "Semaphore wait error: " << strerror(errno);
        return false;
    }
    m_memoryInfo->bufferNumber++;
    if (m_memoryInfo->bufferNumber == m_memoryInfo->bufferNamesCount) {
        m_memoryInfo->bufferNumber = 0;
    }
    auto& [name, descript, pointer, sem] =
        m_memoryBuffer[m_memoryInfo->bufferNumber];

    if (size > m_size) {
        if (ftruncate(descript, size * 2) == -1) {
            std::cout << "Shared Memory Truncate of name: " << name
                      << " failed error: " << strerror(errno) << std::endl;
            return false;
        }
        m_size = size * 2;
    }

    if (sem_wait(sem) == -1) {
        std::cout << "Semaphore wait error: " << strerror(errno);
        return false;
    }
    std::memcpy(pointer, memory, size);
    if (m_memoryInfo->bufferSize != m_size) {
        m_memoryInfo->bufferSize = m_size;
    }
    m_memoryInfo->dataSize = size;
    if (sem_post(sem) == -1) {
        std::cout << "Semaphore post error: " << strerror(errno);
        return false;
    }
    if (sem_post(&m_memoryInfo->semaphore) == -1) {
        std::cout << "Semaphore post error: " << strerror(errno);
        return false;
    }
    return true;
};

#endif

}// namespace SharedMemory
