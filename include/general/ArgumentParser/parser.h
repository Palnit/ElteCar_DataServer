#ifndef ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
#define ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H

#include <string>
#include <vector>
#include "general/ArgumentParser/base_runner.h"
#include "general/ArgumentParser/runner.h"

namespace Arg {

class Parser {
public:
    Parser(int argc, char** argv);
    Parser(int argc, char** argv, std::string description);
    Parser(std::vector<std::string>::iterator begin,
           std::vector<std::string>::iterator end);
    ~Parser();

    template<class T, Arg::RunnerType type>
    void addRunner(Arg::Runner<T, type>* runner) {
        m_runners.push_back(runner);
    };

    void parse();

private:
    std::vector<BaseRunner*> m_runners;
    std::vector<std::string> m_arguments;
    std::string m_description;
};

}// namespace Arg

#endif// ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
