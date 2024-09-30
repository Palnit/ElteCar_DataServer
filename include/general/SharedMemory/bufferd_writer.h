#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H

#include <string>
#include <vector>
#include "general/SharedMemory/info.h"
#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else
#include <semaphore.h>
#endif

namespace SharedMemory {
#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)

class BufferedWriter {

public:
    BufferedWriter(const std::string& infoBufferName,
                   const std::string& defaultBufferName,
                   int bufferCount);

    BufferedWriter(const std::string& infoBufferName,
                   int size,
                   const std::string& defaultBufferName,
                   int bufferCount);

    ~BufferedWriter();

    bool writeMemory(const void* memory, std::size_t size);

    std::string InfoBufferName() { return m_infoBufferName; }

    bool initalize();

private:
    // vector containing name of the buffer, descriptor and the pointer to memory
    std::vector<std::tuple<std::string, std::string, HANDLE, void*, HANDLE>>
        m_memoryBuffer;
    std::string m_infoBufferName;
    SharedMemory::Info* m_memoryInfo;
    HANDLE m_infoSem = nullptr;
    HANDLE m_infoDescriptor = nullptr;
    int m_size;
    int m_infoSize;
    bool m_initalized;
};
#else

class BufferedWriter {

public:
    BufferedWriter(const std::string& infoBufferName,
                   const std::string& defaultBufferName,
                   int bufferCount);

    BufferedWriter(const std::string& infoBufferName,
                   int size,
                   const std::string& defaultBufferName,
                   int bufferCount);

    ~BufferedWriter();

    bool writeMemory(const void* memory, std::size_t size);

    bool initalize();

    std::string InfoBufferName() { return m_infoBufferName; }

private:
    // vector containing name of the buffer, descriptor and the pointer to memory
    std::vector<std::tuple<std::string, int, void*, sem_t*>> m_memoryBuffer;
    std::string m_infoBufferName;
    SharedMemory::Info* m_memoryInfo;
    int m_size;
    int m_infoSize;
    bool m_initalized;
};

#endif
}// namespace SharedMemory

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H
