#include "argument_handler.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

void ArgumentHandler::DelayHandler(std::string input) {
    ArgumentHandler::m_delay = std::stoi(input);
    std::cout << "Got parameter -d/--delay setting delay to " << m_delay
              << std::endl;
}
void ArgumentHandler::BaseImageHandler(std::vector<std::string> inputs) {
    std::cout << "Got parameter -i/--images setting images names:" << std::endl;
    if (inputs[0] == "-dir") {
        std::string baseDir = inputs[1];
        for (auto it = inputs.begin() + 2; it != inputs.end(); ++it) {
            ArgumentHandler::m_imageNames.push_back(baseDir + *it);
            std::cout << ArgumentHandler::m_imageNames.back() << std::endl;
        }
    } else {
        for (auto it = inputs.begin(); it != inputs.end(); ++it) {
            ArgumentHandler::m_imageNames.push_back(*it);
            std::cout << ArgumentHandler::m_imageNames.back() << std::endl;
        }
    }
}
void ArgumentHandler::DataPointHandler(std::string input) {
    ArgumentHandler::m_numberOfDataPoints = std::stoi(input);
    std::cout
        << "Got parameter -m/--max setting maximum number of data points to "
        << m_numberOfDataPoints << std::endl;
}

void ArgumentHandler::CsvHandler(std::string input) {
    ArgumentHandler::m_csvPath = input;
    std::cout << "Got parameter -c/--csvpath setting path of csv file to "
              << ArgumentHandler::m_csvPath << std::endl;
}
void ArgumentHandler::LidarHandler(std::string input) {
    ArgumentHandler::m_lidarPath = input;
    std::cout << "Got parameter -l/--lidar setting path of csv file to "
              << ArgumentHandler::m_lidarPath << std::endl;
}
void ArgumentHandler::OxtHandler(std::string input) {
    ArgumentHandler::m_oxt_file_name = input;
    std::cout << "Got parameter -oxt setting oxt base path and file name to "
              << ArgumentHandler::m_oxt_file_name << std::endl;
}
