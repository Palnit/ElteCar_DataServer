#ifndef ELTECAR_DATASERVER_INCLUDE_CSV_READER_H
#define ELTECAR_DATASERVER_INCLUDE_CSV_READER_H

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

class CSVReader {
public:
    CSVReader(const std::string& path, bool header) : m_header(header) {
        m_fileStream.open(path);
        if (m_header) {
            std::string line;
            std::getline(m_fileStream, line);
            auto tokens = split(line, ';');
            for (int i = 0; i < tokens.size(); i++) {
                m_headerMap[tokens[i]] = i;
            }
        }
    }

    ~CSVReader() { m_fileStream.close(); }

    template<typename... Args>
    bool ReadLine(Args&... args) {
        std::string line;
        if (!std::getline(m_fileStream, line)) { return false; }
        int i = 0;
        std::vector<std::string> splitLine = split(line, ';');
        (
            [&] {
                auto function = getFunctionToString(args);
                args = function(splitLine[i]);
                i++;
            }(),
            ...);
        return true;
    }

    template<typename... Args, std::size_t N>
    bool ReadLine(const std::string (&inputs)[N], Args&... args) {
        static_assert(N == sizeof...(args),
                      "The header names and arguments are not equal");

        std::string line;
        if (!std::getline(m_fileStream, line)) { return false; }
        std::vector<std::string> splitLine = split(line, ';');
        int i = 0;
        (
            [&] {
                auto function = getFunctionToString(args);
                args = function(splitLine[m_headerMap[inputs[i]]]);
                i++;
            }(),
            ...);
        return true;
    }

private:
    std::vector<std::string> split(const std::string& input, char delim) {
        std::vector<std::string> output;
        std::istringstream stream(input);
        std::string token;
        while (std::getline(stream, token, delim)) { output.push_back(token); }
        return output;
    }

    template<typename arg>
    std::function<arg(std::string)> getFunctionToString(arg& args) {
        static_assert(
            std::is_integral_v<std::remove_reference_t<decltype(args)>>
                || std::is_floating_point_v<
                    std::remove_reference_t<decltype(args)>>
                || std::is_assignable_v<decltype(args), std::string>,
            "Cannot convert type from string");

        if constexpr (
            std::is_integral_v<std::remove_reference_t<decltype(args)>>
            && !std::is_same_v<char, std::remove_reference_t<decltype(args)>>
            && !std::is_same_v<std::remove_reference_t<decltype(args)>, bool>) {
            return [](std::string input) { return std::stoi(input); };
        } else if constexpr (std::is_same_v<
                                 bool,
                                 std::remove_reference_t<decltype(args)>>) {
            return [](std::string input) {
                if (input == "true") return true;
                else
                    return false;
                return (bool) std::stoi(input);
            };
        }

        else if constexpr (std::is_same_v<
                               float,
                               std::remove_reference_t<decltype(args)>>) {
            return [](std::string input) { return std::stof(input); };
        } else if constexpr (std::is_same_v<
                                 double,
                                 std::remove_reference_t<decltype(args)>>) {
            return [](std::string input) { return std::stod(input); };
        } else if constexpr (std::is_same_v<
                                 char,
                                 std::remove_reference_t<decltype(args)>>) {
            return [](std::string input) { return input.front(); };
        } else if constexpr (std::is_assignable_v<decltype(args),
                                                  std::string>) {
            return [](std::string input) { return input; };
        }
    }

    std::ifstream m_fileStream;
    std::map<std::string, int> m_headerMap;
    bool m_header;
};

#endif// ELTECAR_DATASERVER_INCLUDE_CSV_READER_H
