#include "argument_handler.h"
#include <iostream>
#include <string>
#include <vector>

void ArgumentHandler::DelayHandler(std::string input) {
    ArgumentHandler::m_delay = std::stoi(input);
    std::cout << "Got parameter -d/--delay setting delay to " << m_delay
              << std::endl;
}
void ArgumentHandler::BaseImageHandler(std::vector<std::string> inputs) {
    std::string baseDir = inputs[0];
    std::cout << "Got parameter -i/--images setting images names:" << std::endl;
    for (auto it = inputs.begin() + 1; it != inputs.end(); it++) {
        ArgumentHandler::m_imageNames.push_back(baseDir + *it);
        std::cout << ArgumentHandler::m_imageNames.back() << std::endl;
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
