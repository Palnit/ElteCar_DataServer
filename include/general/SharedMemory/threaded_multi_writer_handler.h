#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
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
    bool writeMultiMemory(void* data[N], long size[N]) {
        assert(N == m_writers.size());

        if (!m_initalized) {
            if ((m_initalized = initalize()) == false) { return false; }
        }

        if (sem_wait(&m_memoryInfo->semaphore) == -1) {
            std::cout << "Semaphore wait error: " << strerror(errno);
            return false;
        }

        std::vector<std::thread> threads;

        for (int i = 0; i < N; i++) {
            std::thread thread(
                [](SharedMemory::BufferedWriter& writer, void* data,
                   std::size_t size) { writer.writeMemory(data, size); },
                std::ref(m_writers[i]), data[i], size[i]);

            threads.push_back(std::move(thread));
        }
        for (auto& thread : threads) { thread.join(); }

        if (sem_post(&m_memoryInfo->semaphore) == -1) {
            std::cout << "Semaphore post error: " << strerror(errno);
            return false;
        }

        return true;
    }

    bool writeMultiMemory(std::vector<void*> data, std::vector<long> size) {
        assert(data.size() == m_writers.size());
        assert(size.size() == m_writers.size());

        if (!m_initalized) {
            if ((m_initalized = initalize()) == false) { return false; }
        }

        if (sem_wait(&m_memoryInfo->semaphore) == -1) {
            std::cout << "Semaphore wait error: " << strerror(errno);
            return false;
        }

        std::vector<std::thread> threads;

        for (int i = 0; i < data.size(); i++) {
            std::thread thread(
                [](SharedMemory::BufferedWriter& writer, void* data,
                   std::size_t size) { writer.writeMemory(data, size); },
                std::ref(m_writers[i]), data[i], size[i]);

            threads.push_back(std::move(thread));
        }
        for (auto& thread : threads) { thread.join(); }

        if (sem_post(&m_memoryInfo->semaphore) == -1) {
            std::cout << "Semaphore post error: " << strerror(errno);
            return false;
        }

        return true;
    }

private:
    std::vector<SharedMemory::BufferedWriter> m_writers;
    std::string m_infoBufferName;
    SharedMemory::ThreadedInfo* m_memoryInfo;
    bool m_initalized = false;
};

}// namespace SharedMemory

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H
