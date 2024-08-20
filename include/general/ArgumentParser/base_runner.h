#ifndef ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H
#define ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H

#include <string>
#include <vector>

namespace Arg {

enum class RunnerType {
    ARRAY_ARGUMENTS,
    NORMAL_ARGUMENT,
};

class BaseRunner {
public:
    BaseRunner(Arg::RunnerType type) : m_type(type){};
    virtual ~BaseRunner(){};
    Arg::RunnerType type() const { return m_type; }
    virtual void run() {};
    virtual void add(std::vector<std::string>::iterator begin,
                     std::vector<std::string>::iterator end) {};
    virtual void add(std::vector<std::string>::iterator it) {};
    virtual bool testArgument(std::vector<std::string>::iterator it) {
        return false;
    };

private:
    Arg::RunnerType m_type;
};

}// namespace Arg

#endif// ELTECAR_DATASERVER_INCLUDE_GENERAL_ARGUMENTPARSER_BASE_RUNNER_H
