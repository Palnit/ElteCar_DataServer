#ifndef ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H
#define ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H

#include <ios>
#include <utility>
namespace FileHandling {
std::pair<char*, std::streamsize> BinaryReader(std::string path);
}// namespace FileHandling

#endif// ELTECAR_DATASERVER_INCLUDE_BINARY_READER_H
