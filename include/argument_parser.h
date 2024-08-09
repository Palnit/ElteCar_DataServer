#ifndef ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
#define ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H

#include <functional>
#include <string>
#include <vector>

enum class ArgumentRunnerType {
  ARRAY_ARGUMENTS,
  NORMAL_ARGUMENT,
};

class BaseRunner {
public:
  BaseRunner(ArgumentRunnerType type) : m_type(type){};
  virtual ~BaseRunner(){};
  ArgumentRunnerType type() const { return m_type; }
  virtual void run() {};
  virtual void add(std::vector<std::string>::iterator begin,
                   std::vector<std::string>::iterator end) {};
  virtual void add(std::vector<std::string>::iterator it) {};
  virtual bool testArgument(std::vector<std::string>::iterator it) {
    return false;
  };

private:
  ArgumentRunnerType m_type;
};

template <class T, ArgumentRunnerType>
class ArgumentRunner : public BaseRunner {};

template <>
class ArgumentRunner<std::vector<std::string>,
                     ArgumentRunnerType::ARRAY_ARGUMENTS> : public BaseRunner {
public:
  ArgumentRunner(std::string shortName,
                 std::function<void(std::vector<std::string>)> callback)
      : BaseRunner(ArgumentRunnerType::ARRAY_ARGUMENTS), m_shortName(shortName),
        m_callback(callback) {}
  ArgumentRunner(std::string shortName, std::string longName,
                 std::function<void(std::vector<std::string>)> callback)
      : BaseRunner(ArgumentRunnerType::ARRAY_ARGUMENTS), m_shortName(shortName),
        m_longName(longName), m_callback(callback) {}
  void add(std::vector<std::string>::iterator begin,
           std::vector<std::string>::iterator end) override {
    m_data.insert(m_data.end(), begin, end);
  }
  void run() override { m_callback(m_data); }
  bool testArgument(std::vector<std::string>::iterator it) override {
    if (*it == m_shortName) {
      return true;
    }
    if (*it == m_longName) {
      return true;
    }
    return false;
  }

private:
  std::function<void(std::vector<std::string>)> m_callback;
  std::vector<std::string> m_data;
  std::string m_shortName;
  std::string m_longName;
};

template <>
class ArgumentRunner<std::string, ArgumentRunnerType::NORMAL_ARGUMENT>
    : public BaseRunner {
public:
  ArgumentRunner(std::string shortName,
                 std::function<void(std::string)> callback)
      : BaseRunner(ArgumentRunnerType::NORMAL_ARGUMENT), m_shortName(shortName),
        m_callback(callback) {}
  ArgumentRunner(std::string shortName, std::string longName,
                 std::function<void(std::string)> callback)
      : BaseRunner(ArgumentRunnerType::NORMAL_ARGUMENT), m_shortName(shortName),
        m_longName(longName), m_callback(callback) {}
  void add(std::vector<std::string>::iterator it) override { m_data = *it; }
  void run() override { m_callback(m_data); }
  bool testArgument(std::vector<std::string>::iterator it) override {
    if (*it == m_shortName) {
      return true;
    }
    if (*it == m_longName) {
      return true;
    }
    return false;
  }

private:
  std::function<void(std::string)> m_callback;
  std::string m_data;
  std::string m_shortName;
  std::string m_longName;
};

class ArgumentParser {
public:
  ArgumentParser(int argc, char **argv) : m_arguments(argv, argv + argc){};
  ArgumentParser(int argc, char **argv, std::string description)
      : m_arguments(argv, argv + argc), m_description(description){};
  ~ArgumentParser() {
    for (BaseRunner *runner : m_runners) {
      delete runner;
    }
  }

  template <class T, ArgumentRunnerType type>
  void addRunner(ArgumentRunner<T, type> *runner) {
    m_runners.push_back(runner);
  };

  void parse() {
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

private:
  std::vector<BaseRunner *> m_runners;
  std::vector<std::string> m_arguments;
  std::string m_description;
};

#endif // ELTECAR_DATASERVER_INCLUDE_ARGUMENT_PARSER_H
