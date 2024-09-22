#include <semaphore.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>
#include <thread>
#include <vector>
#include "argument_handler.h"
#include "cpp_file_handling.h"
#include "csv_reader.h"
#include "general/ArgumentParser/base_runner.h"
#include "general/ArgumentParser/parser.h"
#include "general/ArgumentParser/runner.h"
#include "general/SharedMemory/bufferd_writer.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

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

    int ID;
    float Lat;
    float Lon;
    float alt;
    float vel;
    float Ax, Ay, Az;
    float Mx, My, Mz;

    CSVReader asd(ArgumentHandler::m_csvPath, true);
    asd.ReadLine({"ID", "Lat", "Lon"}, ID, Lat, Lon);
    std::cout << ID << Lat << Lon;
    /*
    SharedMemory::BufferedWriter writer("Asd", "Asd_", 2);
    SharedMemory::BufferedWriter writer2("Asd2", "Asd2_", 2);
    SharedMemory::BufferedWriter writer3("Asd3", "Asd3_", 2);
    SharedMemory::BufferedWriter writer4("Asd4", "Asd4_", 2);

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
            writer.writeMemory(message, sizeoffile);
            writer2.writeMemory(message2, sizeoffile2);
            writer3.writeMemory(message3, sizeoffile3);
            writer4.writeMemory(message4, sizeoffile4);
            delete message;
            delete message2;
            delete message3;
            delete message4;
        } catch (std::ifstream::failure e) {
            std::cout << "Error" << std::endl;
            continue;
        }
    }
    */
    return 0;
}
