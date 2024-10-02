#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace Arg {
/// \enum RunnerType
/// \brief enum to know what type is the runner and how much data it wants
enum class RunnerType {
    ARRAY_ARGUMENTS,
    NORMAL_ARGUMENT,
};
/// \class BaseRunner
/// \brief The base class for all runners defins the functio that can be used
class BaseRunner {
public:
    /// Base constructor for the runners must be called when inheriting
    /// @param type The type of the runner
    BaseRunner(Arg::RunnerType type) : m_type(type){};
    virtual ~BaseRunner(){};

    /// Returns the type of the runner
    /// @return the type of the runner
    Arg::RunnerType type() const { return m_type; }

    /// When the parser finishes it calls this function to run the runners
    /// internal callback if the switch is found
    virtual void run() {};

    /// Add a vector of data to the argument runner if they are an array type runner
    /// @param begin the begin iterator of the vector
    /// @param end the end iterator of the vector
    virtual void add(std::vector<std::string>::iterator begin,
                     std::vector<std::string>::iterator end) {};

    /// Add a single string value based on iterator to the runner to use when run
    /// @param it the iterator to the string
    virtual void add(std::vector<std::string>::iterator it) {};

    /// Tests if the command switch is handeled by this runner
    /// @param it the command switches itterator
    /// @return true if this runner handels it false if not
    virtual bool testArgument(std::vector<std::string>::iterator it) {
        return false;
    };

    /// Finds the data for command switch if the json contains this switch
    /// @param data the json data
    /// @return the iterator where the pair for this switch is return end if it does not find it
    virtual nlohmann::json::iterator testJsonArgument(nlohmann::json& data) {
        return data.end();
    };

private:
    Arg::RunnerType m_type;
};

}// namespace Arg

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H
