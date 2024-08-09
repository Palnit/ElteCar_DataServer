#include "argument_parser.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  ArgumentParser parser(argc, argv);
  parser.addRunner(new ArgumentRunner<std::vector<std::string>,
                                      ArgumentRunnerType::ARRAY_ARGUMENTS>(
      "-p", "--pictures", [](std::vector<std::string> data) {
        for (std::string arg : data) {
          std::cout << arg << std::endl;
        }
      }));
  parser.addRunner(
      new ArgumentRunner<std::string, ArgumentRunnerType::NORMAL_ARGUMENT>(
          "-s", "--sictures",
          [](std::string data) { std::cout << data << " yay" << std::endl; }));
  parser.parse();
}
