#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H

#include <cassert>
#include <cstddef>
#include <thread>
#include <vector>
#include "general/SharedMemory/bufferd_writer.h"
#include "general/SharedMemory/info.h"
namespace SharedMemory {

class ThreadedMultiWriterHandler {
public:
    ThreadedMultiWriterHandler(std::string bufferName)
        : m_infoBufferName(bufferName) {}

    void addWriter(SharedMemory::BufferedWriter writers);

    bool initalize();

    template<std::size_t N>
    void writeMultiMemory(void* data[N]) {
        assert(N == m_writers.size());
    }

private:
    std::vector<SharedMemory::BufferedWriter> m_writers;
    std::string m_infoBufferName;
    SharedMemory::ThreadedInfo* m_memoryInfo;
    bool m_initalized = false;
};

}// namespace SharedMemory

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H
