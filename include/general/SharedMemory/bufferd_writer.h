#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H

#include <string>
#include <vector>
#include "general/SharedMemory/info.h"

namespace SharedMemory {

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

private:
    // vector containing name of the buffer, descriptor and the pointer to memory
    std::vector<std::tuple<std::string, int, void*, sem_t*>> m_memoryBuffer;
    std::string m_infoBufferName;
    SharedMemory::Info* m_memoryInfo;
    int m_size;
    int m_infoSize;
    bool m_initalized;
};

}// namespace SharedMemory

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_BUFFERD_WRITER_H
