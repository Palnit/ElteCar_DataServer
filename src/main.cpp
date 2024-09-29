#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <regex>
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
            "-c", "-csvpath", &ArgumentHandler::CsvHandler));
    parser.parse();

    CSVReader csvData(ArgumentHandler::m_csvPath, true);
    std::vector<cartesians> csvCartesians;
    cartesians line;
    while (csvData.ReadLine(line.ID, line.Lat, line.Lon, line.Alt, line.Vel,
                            line.Ax, line.Ay, line.Az, line.Mx, line.My,
                            line.Mz)) {
        csvCartesians.push_back(line);
    }

    SharedMemory::BufferedWriter writer("Asd", "Asd_", 2);
    SharedMemory::BufferedWriter writer2("Asd2", "Asd2_", 2);
    SharedMemory::BufferedWriter writer3("Asd3", "Asd3_", 2);
    SharedMemory::BufferedWriter writer4("Asd4", "Asd4_", 2);
    SharedMemory::ThreadedMultiWriterHandler multi("Test");
    multi.addWriter(writer);
    multi.addWriter(writer2);
    multi.addWriter(writer3);
    multi.addWriter(writer4);

    for (int i = 1; i < ArgumentHandler::m_numberOfDataPoints; i++) {
        std::chrono::milliseconds dura(ArgumentHandler::m_delay);
        std::this_thread::sleep_for(dura);
        std::regex regex("\\{.*?\\}");
        std::string name = std::regex_replace(ArgumentHandler::m_imageNames[0],
                                              regex, std::to_string(i));
        std::string name2 = std::regex_replace(ArgumentHandler::m_imageNames[1],
                                               regex, std::to_string(i));
        std::string name3 = std::regex_replace(ArgumentHandler::m_imageNames[2],
                                               regex, std::to_string(i));
        std::string name4 = std::regex_replace(ArgumentHandler::m_imageNames[3],
                                               regex, std::to_string(i));
        std::cout << "reading " << name << std::endl;
        std::cout << name2 << std::endl;
        std::cout << name3 << std::endl;
        std::cout << name4 << std::endl;

        try {
            auto [message, sizeoffile] = FileHandling::BinaryReader(name);
            auto [message2, sizeoffile2] = FileHandling::BinaryReader(name2);
            auto [message3, sizeoffile3] = FileHandling::BinaryReader(name3);
            auto [message4, sizeoffile4] = FileHandling::BinaryReader(name4);

            void* data[4] = {message, message2, message3, message4};
            long size[4] = {sizeoffile, sizeoffile2, sizeoffile3, sizeoffile4};

            multi.writeMultiMemory<4>(data, size);

            delete message;
            delete message2;
            delete message3;
            delete message4;
        } catch (std::ifstream::failure e) {
            std::cout << "Error" << std::endl;
            continue;
        }
    }
    return 0;
}
