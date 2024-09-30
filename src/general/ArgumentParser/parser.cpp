#include "general/ArgumentParser/parser.h"
#include <fstream>
#include <iostream>
#include "general/ArgumentParser/base_runner.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace Arg {

void Parser::parse() {
    if (m_arguments.size() < 3) {
        if (m_defaultRunner) {
            m_defaultRunner->run();
        } else {
            std::ifstream stream;
            stream.open("./input.json");
            if (stream.fail()) { return; }
            nlohmann::json data = nlohmann::json::parse(stream);
            for (BaseRunner* runner : m_runners) {
                nlohmann::json::iterator it;
                if ((it = runner->testJsonArgument(data)) == data.end()) {
                    continue;
                }
                if (runner->type() == Arg::RunnerType::ARRAY_ARGUMENTS) {
                    std::vector<std::string> data;
                    std::cout << *it << std::endl;
                    for (auto to_string : *it) { data.push_back(to_string); }
                    runner->add(data.begin(), data.end());
                }
                if (runner->type() == Arg::RunnerType::NORMAL_ARGUMENT) {
                    std::vector<std::string> data;
                    data.push_back(it->dump());
                    runner->add(data.begin());
                }
            }
        }
        for (BaseRunner* runner : m_runners) { runner->run(); }
        return;
    }
    std::vector<std::string>::iterator tmpBegin;
    BaseRunner* arrayRunner = nullptr;
    for (std::vector<std::string>::iterator it = m_arguments.begin();
         it != m_arguments.end(); it++) {
        if (arrayRunner != nullptr && (it + 1) == m_arguments.end()) {
            arrayRunner->add(tmpBegin, m_arguments.end());
            continue;
        }
        for (BaseRunner* runner : m_runners) {
            if (runner->testArgument(it)
                && runner->type() == Arg::RunnerType::ARRAY_ARGUMENTS) {
                if (arrayRunner != nullptr) {
                    arrayRunner->add(tmpBegin, it);
                    arrayRunner = nullptr;
                }
                tmpBegin = ++it;
                arrayRunner = runner;
            }
            if (runner->testArgument(it)
                && runner->type() == Arg::RunnerType::NORMAL_ARGUMENT) {
                if (arrayRunner != nullptr) {
                    arrayRunner->add(tmpBegin, it);
                    arrayRunner = nullptr;
                }
                runner->add(++it);
            }
        }
    }
    for (BaseRunner* runner : m_runners) { runner->run(); }
}

Parser::Parser(int argc, char** argv) : m_arguments(argv, argv + argc){};

Parser::Parser(int argc, char** argv, std::string description)
    : m_arguments(argv, argv + argc),
      m_description(description){};

Parser::~Parser() {
    for (BaseRunner* runner : m_runners) { delete runner; }
    if (m_defaultRunner) { delete m_defaultRunner; }
}

Parser::Parser(std::vector<std::string>::iterator begin,
               std::vector<std::string>::iterator end)
    : m_arguments(begin, end) {}
}// namespace Arg
