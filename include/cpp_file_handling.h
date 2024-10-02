#ifndef ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H
#define ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H

#include <ios>
#include <utility>
namespace FileHandling {

/// Reads a file as a binary data
/// @param path the path to the file
/// @return a pair with the binary data pointer and the size of the data
std::pair<char*, std::streamsize> BinaryReader(std::string path);
}// namespace FileHandling

#endif// ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H
