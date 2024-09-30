#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "argument_handler.h"
#include "cartesians.h"
#include "cpp_file_handling.h"
#include "csv_reader.h"
#include "general/ArgumentParser/base_runner.h"
#include "general/ArgumentParser/parser.h"
#include "general/ArgumentParser/runner.h"
#include "general/SharedMemory/bufferd_writer.h"
#include "general/SharedMemory/threaded_multi_writer_handler.h"
#include "lidar_data.h"

std::string numberFile(std::string input, int i) {
    std::regex regex("\\{.*?\\}");
    return std::regex_replace(input, regex, std::to_string(i));
}

std::vector<LidarData> readLidar(std::string fileName) {
    std::vector<LidarData> output;
    std::ifstream stream;
    stream.open(fileName);
    std::string line;
    while (std::getline(stream, line)) {
        std::stringstream stringstream(line);
        std::string data_line;
        std::vector<std::string> data;
        while (std::getline(stringstream, data_line, ' ')) {
            data.push_back(data_line);
        }
        if (data.size() < 4) { return std::vector<LidarData>(); }
        LidarData lidar;
        lidar.x = std::stod(data[0]);
        lidar.y = std::stod(data[1]);
        lidar.z = std::stod(data[2]);
        lidar.reflect = std::stoi(data[3]);
        output.push_back(lidar);
    }
    stream.close();
    return output;
};

int main(int argc, char** argv) {
    Arg::Parser parser(argc, argv);
    parser.addRunner(
        new Arg::Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>(
            "-d", "--delay", &ArgumentHandler::DelayHandler));
    parser.addRunner(
        new Arg::Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>(
            "-m", "--max", &ArgumentHandler::DataPointHandler));

    parser.addRunner(new Arg::Runner<std::vector<std::string>,
                                     Arg::RunnerType::ARRAY_ARGUMENTS>(
        "-i", "--images", &ArgumentHandler::BaseImageHandler));
    parser.addRunner(
        new Arg::Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>(
            "-c", "--csvpath", &ArgumentHandler::CsvHandler));
    parser.addRunner(
        new Arg::Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>(
            "-l", "--lidar", &ArgumentHandler::LidarHandler));
    parser.parse();

    SharedMemory::ThreadedMultiWriterHandler multi("Images");
    SharedMemory::BufferedWriter writer("Lidar", "Lidar_", 2);
    SharedMemory::BufferedWriter csvwriter("Csv", "Csv_", 2);

    std::string writer_name = "Writer";
    for (int i = 0; i < ArgumentHandler::m_imageNames.size(); ++i) {

        multi.addWriter(SharedMemory::BufferedWriter(
            writer_name + std::to_string(i),
            writer_name + "_" + std::to_string(i), 2));
    }

    CSVReader csvData(ArgumentHandler::m_csvPath, true);
    std::vector<cartesians> csvCartesians;
    cartesians line;
    while (csvData.ReadLine(line.ID, line.Lat, line.Lon, line.Alt, line.Vel,
                            line.Ax, line.Ay, line.Az, line.Mx, line.My,
                            line.Mz)) {
        csvCartesians.push_back(line);
    }

    for (int i = 1; i < ArgumentHandler::m_numberOfDataPoints; i++) {
        std::chrono::milliseconds dura(ArgumentHandler::m_delay);
        std::this_thread::sleep_for(dura);
        std::vector<void*> data;
        std::vector<long> size;
        std::cout << "Reading Images:";
        try {
            for (auto name : ArgumentHandler::m_imageNames) {
                auto trueName = numberFile(name, i);
                std::cout << trueName << std::endl;
                auto [message, sizeoffile] =
                    FileHandling::BinaryReader(numberFile(trueName, i));
                data.push_back(message);
                size.push_back(sizeoffile);
            }
            multi.writeMultiMemory(data, size);
            for (auto message : data) { delete (char*) message; }
        } catch (std::ifstream::failure e) {
            std::cout << "Error" << std::endl;
            continue;
        }
        std::string lidarTruePath = numberFile(ArgumentHandler::m_lidarPath, i);
        std::cout << "Reading Lidar Data:" << lidarTruePath << std::endl;
        std::vector<LidarData> lidarData = readLidar(lidarTruePath);
        writer.writeMemory(lidarData.data(),
                           sizeof(LidarData) * lidarData.size());
        csvwriter.writeMemory(&csvCartesians[i], sizeof(cartesians));
    }
    return 0;
}
