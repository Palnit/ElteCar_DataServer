#include "general/SharedMemory/bufferd_writer.h"
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include "general/SharedMemory/info.h"

namespace SharedMemory {

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
    if (sem_init(&(m_memoryInfo->semaphore), 1, 0) == -1) {
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

        if (sem_init(semaphore, 1, 0) == -1) {
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
    m_initalized = true;
    return true;
}

void* BufferedWriter::getMemoryPointer() {
    if (!m_initalized) { initalize(); }

    return std::get<2>(m_memoryBuffer[m_memoryInfo->bufferNumber]);
}
BufferedWriter::~BufferedWriter() {
    shm_unlink(m_infoBufferName.c_str());
    for (auto& [name, descript, pointer, sem] : m_memoryBuffer) {
        shm_unlink(name.c_str());
    }
}
}// namespace SharedMemory
