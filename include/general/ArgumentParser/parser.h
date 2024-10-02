#ifndef ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
#define ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H

#include <string>
#include <vector>
#include "general/ArgumentParser/base_runner.h"
#include "general/ArgumentParser/runner.h"

namespace Arg {

/// \class Parser
/// \brief Class used to parse the input values from the cmd
///
/// this class uses the runner classes to handel different kind of input args switches
/// they must be added before parsing to use if you add a runner it takes control of it
/// and delets them when the Parser is deleted
class Parser {
public:
    /// Constructor
    /// @param argc argument size
    /// @param argv arguments
    Parser(int argc, char** argv);

    /// Constructor
    /// @param argc argument size
    /// @param argv arguments
    /// @param description desciption for the program TODO
    Parser(int argc, char** argv, std::string description);

    /// Constructor
    /// @param begin begin iterator for a vector of string data
    /// @param end end iterator for a vector of string data
    Parser(std::vector<std::string>::iterator begin,
           std::vector<std::string>::iterator end);

    /// Deconstructor delets runners as well
    ~Parser();

    /// Add a runner to use when parsing
    /// @tparam T The callback type of the runner
    /// @tparam type the type of the runner
    /// @param runner the argparser runner
    template<class T, Arg::RunnerType type>
    void addRunner(Arg::Runner<T, type>* runner) {
        m_runners.push_back(runner);
    };

    /// Add a default runner to run when theres not enough arguments
    /// @tparam T The callback type of the runner
    /// @tparam type the type of the runner
    /// @param runner the argparser runner
    template<class T, Arg::RunnerType type>
    void addDefaultRunner(Arg::Runner<T, type>* runner) {
        if (m_defaultRunner) { delete m_defaultRunner; }
        m_defaultRunner = runner;
    };

    /// parses the argument list and calss the runners to use their callbacks at the end
    void parse();

private:
    std::vector<BaseRunner*> m_runners;
    BaseRunner* m_defaultRunner = nullptr;
    std::vector<std::string> m_arguments;
    std::string m_description;
};

}// namespace Arg

#endif// ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
