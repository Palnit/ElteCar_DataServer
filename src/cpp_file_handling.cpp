#include "cpp_file_handling.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

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
Cartesians readCartesiansFromOxt(const std::string& path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(path);
    std::stringstream data;
    data << ifs.rdbuf();
    std::vector<std::string> split_data;
    std::string s;
    Cartesians cart{};
    while (getline(data, s, ' ')) { split_data.push_back(s); }
    cart.Lat = std::stod(split_data[0]);
    cart.Lon = std::stod(split_data[1]);
    cart.Alt = std::stod(split_data[2]);
    cart.Roll = std::stod(split_data[3]);
    cart.Pitch = std::stod(split_data[4]);
    cart.Yaw = std::stod(split_data[5]);
    ifs.close();
    return cart;
}

}// namespace FileHandling
