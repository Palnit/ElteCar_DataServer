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


#if defined(WIN32) || defined(_WIN32) \
|| defined(__WIN32) && !defined(__CYGWIN__)
#else
#include <sys/mman.h>
#endif
namespace SharedMemory {

#if defined(WIN32) || defined(_WIN32) \
    || defined(__WIN32) && !defined(__CYGWIN__)

/// \class ThreadedMultiWriterHandler
/// \brief This class splits multiple buffered writers into separate threads
/// and syncs them tougether
class ThreadedMultiWriterHandler {
public:

    /// Constructor
    /// @param bufferName Name of the threaded writers info buffer
    ThreadedMultiWriterHandler(std::string bufferName)
        : m_infoBufferName(bufferName) {}

    /// Deconstructor to close shared memory
    ~ThreadedMultiWriterHandler();

    /// Add a buffered writer to use in a threaded way
    /// @param writers the writter to be added copies it
    void addWriter(SharedMemory::BufferedWriter writers);

    /// initializes the threader writers shared memory and semaphore
    bool initalize();

    /// Writes to memory an array of data
    /// @tparam N The number of data in the array
    /// @param data The data array to write into the order must be the same that the
    /// writers were added to the threaded writer
    /// @param size the size of each data
    /// @return whatever the operation succeded or not
    template<std::size_t N>
    bool writeMultiMemory(void* data[N], long size[N]) {
        assert(N == m_writers.size());

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

        std::vector<std::thread> threads;

        for (int i = 0; i < N; i++) {
            std::thread thread(
                [](SharedMemory::BufferedWriter& writer, void* data,
                   std::size_t size) { writer.writeMemory(data, size); },
                std::ref(m_writers[i]), data[i], size[i]);

            threads.push_back(std::move(thread));
        }
        for (auto& thread : threads) { thread.join(); }

        if (!ReleaseSemaphore(m_infoSem, 1, nullptr)) {
            std::cout << "Semaphore post error: " << GetLastError();
            return false;
        }

        return true;
    }

    /// Writes to memory an vecotr of data
    /// @param data The data vector to write into the order must be the same that the
    /// writers were added to the threaded writer
    /// @param size the size of each data
    /// @return whatever the operation succeded or not
    bool writeMultiMemory(std::vector<void*> data, std::vector<long> size) {
        assert(data.size() == m_writers.size());
        assert(size.size() == m_writers.size());

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

        std::vector<std::thread> threads;

        for (int i = 0; i < data.size(); i++) {
            std::thread thread(
                [](SharedMemory::BufferedWriter& writer, void* data,
                   std::size_t size) { writer.writeMemory(data, size); },
                std::ref(m_writers[i]), data[i], size[i]);

            threads.push_back(std::move(thread));
        }
        for (auto& thread : threads) { thread.join(); }

        if (!ReleaseSemaphore(m_infoSem, 1, nullptr)) {
            std::cout << "Semaphore post error: " << GetLastError();
            return false;
        }

        return true;
    }

private:
    std::vector<SharedMemory::BufferedWriter> m_writers;
    std::string m_infoBufferName;
    SharedMemory::ThreadedInfo* m_memoryInfo;
    HANDLE m_infoSem = nullptr;
    HANDLE m_infoDescriptor = nullptr;
    bool m_initalized = false;
};
#else

/// \class ThreadedMultiWriterHandler
/// \brief This class splits multiple buffered writers into separate threads
/// and syncs them tougether
class ThreadedMultiWriterHandler {
public:
    /// Constructor
    /// @param bufferName Name of the threaded writers info buffer
    ThreadedMultiWriterHandler(std::string bufferName)
        : m_infoBufferName(bufferName) {}

    ~ThreadedMultiWriterHandler(){ shm_unlink(m_infoBufferName.c_str());}

    /// Add a buffered writer to use in a threaded way
    /// @param writers the writter to be added copies it
    void addWriter(SharedMemory::BufferedWriter writers);

    /// initializes the threader writers shared memory and semaphore
    bool initalize();

    /// Writes to memory an array of data
    /// @tparam N The number of data in the array
    /// @param data The data array to write into the order must be the same that the
    /// writers were added to the threaded writer
    /// @param size the size of each data
    /// @return whatever the operation succeded or not
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

    /// Writes to memory an vecotr of data
    /// @param data The data vector to write into the order must be the same that the
    /// writers were added to the threaded writer
    /// @param size the size of each data
    /// @return whatever the operation succeded or not
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
#endif
}// namespace SharedMemory

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_SHAREDMEMORY_THREADED_MULTI_WRITER_HANDLER_H
