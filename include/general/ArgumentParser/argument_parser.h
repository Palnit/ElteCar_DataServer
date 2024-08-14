#ifndef ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
#define ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H

#include "general/ArgumentParser/argument_runner.h"
#include "general/ArgumentParser/base_runner.h"
#include <string>
#include <vector>

class ArgumentParser {
public:
  ArgumentParser(int argc, char **argv);
  ArgumentParser(int argc, char **argv, std::string description);
  ~ArgumentParser();

  template <class T, ArgumentRunnerType type>
  void addRunner(ArgumentRunner<T, type> *runner) {
    m_runners.push_back(runner);
  };

  void parse();

private:
  std::vector<BaseRunner *> m_runners;
  std::vector<std::string> m_arguments;
  std::string m_description;
};

#endif // ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
