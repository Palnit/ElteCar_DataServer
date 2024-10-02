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

/// \class BufferedWriter
/// \brief This class is used to write data into a shared memory
///
/// this class writes any type of data into a shard memory and automaticly buffers
class BufferedWriter {
public:
    /// Constructor
    /// @param infoBufferName The name of the info buffer for this writer the
    /// reader need this to read from the same shared memory
    /// @param defaultBufferName The default name to use for the buffers
    /// shared memory the number of the buffer is added to the end for the name
    /// @param bufferCount the number of buffers you want to use
    BufferedWriter(const std::string& infoBufferName,
                   const std::string& defaultBufferName,
                   int bufferCount);

    /// Constructor
    /// @param infoBufferName The name of the info buffer for this writer the
    /// reader need this to read from the same shared memory
    /// @param size The size of the info buffer to use by default if the size is
    /// not enough its incrased TODO
    /// @param defaultBufferName The default name to use for the buffers
    /// shared memory the number of the buffer is added to the end for the name
    /// @param bufferCount the number of buffers you want to use
    BufferedWriter(const std::string& infoBufferName,
                   int size,
                   const std::string& defaultBufferName,
                   int bufferCount);

    /// Destructor to take care of closing shared memory
    ~BufferedWriter();

    /// Function to write a data into a shared memory
    /// @param memory The pointer to the data can be any type of data
    /// @param size The size of the data
    /// @return whatever the operation was succesfull or not
    bool writeMemory(const void* memory, std::size_t size);

    /// initalizes the buffers
    /// @return whatever the operation was sucessfull or not
    bool initalize();

    /// Returns the name for the info buffer accosited whit this writer
    /// @return the name of the info buffer
    std::string InfoBufferName() { return m_infoBufferName; }

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

/// \class BufferedWriter
/// \brief This class is used to write data into a shared memory
///
/// this class writes any type of data into a shard memory and automaticly buffers
class BufferedWriter {
public:
    /// Constructor
    /// @param infoBufferName The name of the info buffer for this writer the
    /// reader need this to read from the same shared memory
    /// @param defaultBufferName The default name to use for the buffers
    /// shared memory the number of the buffer is added to the end for the name
    /// @param bufferCount the number of buffers you want to use
    BufferedWriter(const std::string& infoBufferName,
                   const std::string& defaultBufferName,
                   int bufferCount);

    /// Constructor
    /// @param infoBufferName The name of the info buffer for this writer the
    /// reader need this to read from the same shared memory
    /// @param size The size of the info buffer to use by default if the size is
    /// not enough its incrased
    /// @param defaultBufferName The default name to use for the buffers
    /// shared memory the number of the buffer is added to the end for the name
    /// @param bufferCount the number of buffers you want to use
    BufferedWriter(const std::string& infoBufferName,
                   int size,
                   const std::string& defaultBufferName,
                   int bufferCount);

    /// Destructor to take care of closing shared memory
    ~BufferedWriter();

    /// Function to write a data into a shared memory
    /// @param memory The pointer to the data can be any type of data
    /// @param size The size of the data
    /// @return whatever the operation was succesfull or not
    bool writeMemory(const void* memory, std::size_t size);

    /// initalizes the buffers
    /// @return whatever the operation was sucessfull or not
    bool initalize();

    /// Returns the name for the info buffer accosited whit this writer
    /// @return the name of the info buffer
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
