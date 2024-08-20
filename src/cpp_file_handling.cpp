#include "cpp_file_handling.h"

#include <fstream>

namespace FileHandling {

std::pair<char*, std::streamsize> BinaryReader(std::string path) {

    std::ifstream ifs;
    char* data;
    std::streamsize sizeOfFile;

    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(path, std::ios::binary | std::ios::in | std::ios::ate);
    sizeOfFile = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    data = new char[sizeOfFile];
    ifs.read(data, sizeOfFile);
    ifs.close();
    return {data, sizeOfFile};
}

}// namespace FileHandling
