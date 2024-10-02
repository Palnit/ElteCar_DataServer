#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER

#include <functional>
#include "general/ArgumentParser/base_runner.h"
#include "nlohmann/json_fwd.hpp"

namespace Arg {

/// \class Runner
/// \brief This class is used to do something if a switch from the cmd exists
/// @tparam T the input value from the runner only handels strings right now
template<class T, Arg::RunnerType>
class Runner : public BaseRunner {};

/// \class Runner
/// \brief this is a template specilization for the Array_arguments in a runner
template<>
class Runner<std::vector<std::string>, Arg::RunnerType::ARRAY_ARGUMENTS>
    : public BaseRunner {
public:
    /// Constructor
    /// @param shortName the short name of the switch tipicly prepended by a -
    /// @param callback the callback class thats called when the switch is found
    Runner(std::string shortName,
           std::function<void(std::vector<std::string>)> callback)
        : BaseRunner(Arg::RunnerType::ARRAY_ARGUMENTS),
          m_shortName(shortName),
          m_callback(callback) {}

    /// Constructor
    /// @param shortName the short name of the switch tipicly prepended by a -
    /// @param longName the long name of the switch tipicly prepended by --
    /// @param callback the callback class thats called when the switch is found
    Runner(std::string shortName,
           std::string longName,
           std::function<void(std::vector<std::string>)> callback)
        : BaseRunner(Arg::RunnerType::ARRAY_ARGUMENTS),
          m_shortName(shortName),
          m_longName(longName),
          m_callback(callback) {}

    /// Add a vector of data to the argument runner if they are an array type runner
    /// @param begin the begin iterator of the vector
    /// @param end the end iterator of the vector
    void add(std::vector<std::string>::iterator begin,
             std::vector<std::string>::iterator end) override {
        m_data.insert(m_data.end(), begin, end);
    }

    /// When the parser finishes it calls this function to run the runners
    /// internal callback if the switch is found
    void run() override {
        if (!m_data.empty()) { m_callback(m_data); }
    }

    /// Tests if the command switch is handeled by this runner
    /// @param it the command switches itterator
    /// @return true if this runner handels it false if not
    bool testArgument(std::vector<std::string>::iterator it) override {
        if (*it == m_shortName) { return true; }
        if (*it == m_longName) { return true; }
        return false;
    }

    /// Finds the data for command switch if the json contains this switch
    /// @param data the json data
    /// @return the iterator where the pair for this switch is return end if it does not find it
    nlohmann::json::iterator testJsonArgument(nlohmann::json& data) override {
        nlohmann::json::iterator it;
        if ((it = data.find(m_shortName)) != data.end()) { return it; }
        if ((it = data.find(m_longName)) != data.end()) { return it; }
        return data.end();
    }

private:
    std::function<void(std::vector<std::string>)> m_callback;
    std::vector<std::string> m_data;
    std::string m_shortName;
    std::string m_longName;
};

/// \class Runner
/// \brief this is a template specilization for the Normal_arguments in a runner
template<>
class Runner<std::string, Arg::RunnerType::NORMAL_ARGUMENT>
    : public BaseRunner {
public:

    /// Constructor
    /// @param shortName the short name of the switch tipicly prepended by a -
    /// @param callback the callback class thats called when the switch is found
    Runner(std::string shortName, std::function<void(std::string)> callback)
        : BaseRunner(Arg::RunnerType::NORMAL_ARGUMENT),
          m_shortName(shortName),
          m_callback(callback) {}

    /// Constructor
    /// @param shortName the short name of the switch tipicly prepended by a -
    /// @param longName the long name of the switch tipicly prepended by --
    /// @param callback the callback class thats called when the switch is found
    Runner(std::string shortName,
           std::string longName,
           std::function<void(std::string)> callback)
        : BaseRunner(Arg::RunnerType::NORMAL_ARGUMENT),
          m_shortName(shortName),
          m_longName(longName),
          m_callback(callback) {}

    /// Add a single string value based on iterator to the runner to use when run
    /// @param it the iterator to the string
    void add(std::vector<std::string>::iterator it) override { m_data = *it; }

    /// When the parser finishes it calls this function to run the runners
    /// internal callback if the switch is found
    void run() override {
        if (!m_data.empty()) { m_callback(m_data); }
    }

    /// Tests if the command switch is handeled by this runner
    /// @param it the command switches itterator
    /// @return true if this runner handels it false if not
    bool testArgument(std::vector<std::string>::iterator it) override {
        if (*it == m_shortName) { return true; }
        if (*it == m_longName) { return true; }
        return false;
    }

    /// Finds the data for command switch if the json contains this switch
    /// @param data the json data
    /// @return the iterator where the pair for this switch is return end if it does not find it
    nlohmann::json::iterator testJsonArgument(nlohmann::json& data) override {
        nlohmann::json::iterator it;
        if ((it = data.find(m_shortName)) != data.end()) { return it; }
        if ((it = data.find(m_longName)) != data.end()) { return it; }
        return data.end();
    }

private:
    std::function<void(std::string)> m_callback;
    std::string m_data;
    std::string m_shortName;
    std::string m_longName;
};

}// namespace Arg
#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_ARGUMENT_RUNNER
