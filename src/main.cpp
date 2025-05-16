#include <algorithm>
#include <chrono>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <random>
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

/// fuction to number files gotten from the cmd
/// @param input the input file name
/// @param i the number of the file
/// @return the numbered name of the file
std::string numberFile(std::string input, int i) {
    std::regex numbered_regex(R"(\{N(\d*?)\})");
    std::smatch number_match;
    if (std::regex_search(input, number_match, numbered_regex)) {
        std::stringstream numbering;
        int width = std::atoi(number_match[1].str().c_str());
        numbering << std::setfill('0') << std::setw(width) << i;
        std::string number = numbering.str();
        return std::regex_replace(input, numbered_regex, number);
    }
    std::regex regex("\\{.*?\\}");
    return std::regex_replace(input, regex, std::to_string(i));
}

/// function to read a lidar file into a vector
/// @param fileName the name of the lidar file
/// @param openmode the file open mode
/// @return the output vector
std::vector<LidarData> readLidar(std::string fileName) {
    std::cout << "fileName: " << fileName << std::endl;
    std::vector<LidarData> output;
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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

std::vector<LidarData> readLidarBinary(std::string fileName) {
    std::vector<LidarData> output;
    std::ifstream stream;
    stream.exceptions(std::ifstream::badbit);
    stream.open(fileName, std::ios::binary | std::ios::in);
    stream.seekg(0, std::ios::end);
    const size_t num_elements = stream.tellg() / sizeof(float);
    stream.seekg(0, std::ios::beg);
    std::vector<float> data(num_elements);
    stream.read(reinterpret_cast<char*>(&data[0]),
                num_elements * sizeof(float));
    for (size_t i = 0; i < data.size(); i += 4) {
        LidarData lidar;
        lidar.x = data[i];
        lidar.y = data[i + 1];
        lidar.z = data[i + 2];
        lidar.reflect = 0;
        output.push_back(lidar);
    }
    stream.close();
    return output;
}

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
    parser.addRunner(
        new Arg::Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>(
            "-oxt", "--oxt", &ArgumentHandler::OxtHandler));
    parser.parse();

    SharedMemory::ThreadedMultiWriterHandler multi("Images");
    SharedMemory::BufferedWriter writer("Lidar", 5000000, "Lidar_", 2);
    SharedMemory::BufferedWriter csvwriter("Csv", "Csv_", 2);

    std::string writer_name = "Writer";
    for (int i = 0; i < ArgumentHandler::m_imageNames.size(); ++i) {

        multi.addWriter(SharedMemory::BufferedWriter(
            writer_name + std::to_string(i),
            writer_name + "_" + std::to_string(i), 2));
    }

    std::vector<Cartesians> csvCartesians;
    if (!ArgumentHandler::m_csvPath.empty()) {

        CSVReader csvData(ArgumentHandler::m_csvPath, true);
        Cartesians line{};
        while (csvData.ReadLine(line.ID, line.Lat, line.Lon, line.Alt, line.Vel,
                                line.Ax, line.Ay, line.Az, line.Mx, line.My,
                                line.Mz)) {
            csvCartesians.push_back(line);
        }
    }

    for (int i = 1; i < ArgumentHandler::m_numberOfDataPoints; i++) {
        std::chrono::milliseconds dura(ArgumentHandler::m_delay);
        std::this_thread::sleep_for(dura);
        std::vector<void*> data;
        std::vector<long> size;
        std::vector<LidarData> lidarData;
        std::cout << "Reading Images:" << std::endl;
        try {
            for (const auto& name : ArgumentHandler::m_imageNames) {
                auto trueName = numberFile(name, i);
                std::cout << trueName << std::endl;
                auto [message, sizeoffile] =
                    FileHandling::BinaryReader(numberFile(trueName, i));
                data.push_back(message);
                size.push_back(sizeoffile);
            }
        } catch (std::ifstream::failure& e) {
            std::cout << "Error: no picture number: " << i
                      << "exception: " << e.what() << std::endl;
            continue;
        }
        std::string lidarTruePath = numberFile(ArgumentHandler::m_lidarPath, i);
        Cartesians cart{};
        auto it = std::find_if(csvCartesians.begin(), csvCartesians.end(),
                               [&i](const Cartesians& x) { return x.ID == i; });
        if (it == csvCartesians.end()) {
            if (!ArgumentHandler::m_oxt_file_name.empty()) {
                try {
                    std::string oxtPath =
                        numberFile(ArgumentHandler::m_oxt_file_name, i);
                    cart = FileHandling::readCartesiansFromOxt(oxtPath);
                } catch (std::ifstream::failure& e) {
                    std::cout << "Error: no imu data number: " << i
                              << std::endl;
                    continue;
                }
            } else {
                std::cout << "Error: no imu data number: " << i << std::endl;
            }
        } else {
            cart = *it;
        }
        std::default_random_engine gen;
        std::normal_distribution<double> dist(0.0, 0.0005);
        cart.Lat += dist(gen);
        cart.Lon += dist(gen);
        cart.Yaw += dist(gen);
        cart.Pitch += dist(gen);
        std::cout << "Reading Lidar Data:" << lidarTruePath << std::endl;
        try {
            if (lidarTruePath.ends_with(".bin")) {
                lidarData = readLidarBinary(lidarTruePath);
            } else {
                lidarData = readLidar(lidarTruePath);
            }
        } catch (std::ifstream::failure& e) {
            std::cout << "Error: no lidar: " << i << " exception: " << e.what()
                      << std::endl;

            continue;
        }
        multi.writeMultiMemory(data, size);
        for (auto message : data) { delete (char*) message; }
        writer.writeMemory(lidarData.data(),
                           sizeof(LidarData) * lidarData.size());
        csvwriter.writeMemory(&cart, sizeof(Cartesians));
    }
    return 0;
}
