#include "general/ArgumentParser/argument_parser.h"

void ArgumentParser::parse() {
  std::vector<std::string>::iterator tmpBegin;
  BaseRunner *arrayRunner = nullptr;
  for (std::vector<std::string>::iterator it = m_arguments.begin();
       it != m_arguments.end(); it++) {
    if (arrayRunner != nullptr && (it + 1) == m_arguments.end()) {
      arrayRunner->add(tmpBegin, m_arguments.end());
      continue;
    }
    for (BaseRunner *runner : m_runners) {
      if (runner->testArgument(it) &&
          runner->type() == ArgumentRunnerType::ARRAY_ARGUMENTS) {
        if (arrayRunner != nullptr) {
          arrayRunner->add(tmpBegin, it);
          arrayRunner = nullptr;
        }
        tmpBegin = ++it;
        arrayRunner = runner;
      }
      if (runner->testArgument(it) &&
          runner->type() == ArgumentRunnerType::NORMAL_ARGUMENT) {
        if (arrayRunner != nullptr) {
          arrayRunner->add(tmpBegin, it);
          arrayRunner = nullptr;
        }
        runner->add(++it);
      }
    }
  }
  for (BaseRunner *runner : m_runners) {
    runner->run();
  }
}

ArgumentParser::ArgumentParser(int argc, char **argv)
    : m_arguments(argv, argv + argc){};

ArgumentParser::ArgumentParser(int argc, char **argv, std::string description)
    : m_arguments(argv, argv + argc), m_description(description){};

ArgumentParser::~ArgumentParser() {
  for (BaseRunner *runner : m_runners) {
    delete runner;
  }
}
