#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER

#include <functional>
#include "general/ArgumentParser/base_runner.h"

namespace Arg {

template<class T, Arg::RunnerType>
class Runner : public BaseRunner {};

template<>
class Runner<std::vector<std::string>, Arg::RunnerType::ARRAY_ARGUMENTS>
    : public BaseRunner {
public:
    Runner(std::string shortName,
           std::function<void(std::vector<std::string>)> callback)
        : BaseRunner(Arg::RunnerType::ARRAY_ARGUMENTS),
          m_shortName(shortName),
          m_callback(callback) {}
    Runner(std::string shortName,
           std::string longName,
           std::function<void(std::vector<std::string>)> callback)
        : BaseRunner(Arg::RunnerType::ARRAY_ARGUMENTS),
          m_shortName(shortName),
          m_longName(longName),
          m_callback(callback) {}
    void add(std::vector<std::string>::iterator begin,
             std::vector<std::string>::iterator end) override {
        m_data.insert(m_data.end(), begin, end);
    }
    void run() override { m_callback(m_data); }
    bool testArgument(std::vector<std::string>::iterator it) override {
        if (*it == m_shortName) { return true; }
        if (*it == m_longName) { return true; }
        return false;
    }

private:
    std::function<void(std::vector<std::string>)> m_callback;
    std::vector<std::string> m_data;
    std::string m_shortName;
    std::string m_longName;
};

template<>
class Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>
    : public BaseRunner {
public:
    Runner(std::string shortName, std::function<void(std::string)> callback)
        : BaseRunner(Arg::RunnerType::NORMAL_ARGUMENT),
          m_shortName(shortName),
          m_callback(callback) {}
    Runner(std::string shortName,
           std::string longName,
           std::function<void(std::string)> callback)
        : BaseRunner(Arg::RunnerType::NORMAL_ARGUMENT),
          m_shortName(shortName),
          m_longName(longName),
          m_callback(callback) {}
    void add(std::vector<std::string>::iterator it) override { m_data = *it; }
    void run() override { m_callback(m_data); }
    bool testArgument(std::vector<std::string>::iterator it) override {
        if (*it == m_shortName) { return true; }
        if (*it == m_longName) { return true; }
        return false;
    }

private:
    std::function<void(std::string)> m_callback;
    std::string m_data;
    std::string m_shortName;
    std::string m_longName;
};

}// namespace Arg
#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER
