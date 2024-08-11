#include "argument_parser.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

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

  const int size = 4096;
  std::string message = "hello shared memory";
  uint shmFileDescriptor = shm_open("Test", O_CREAT | O_RDWR, 0666);
  ftruncate(shmFileDescriptor, size);
  void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shmFileDescriptor, 0);
  memcpy(ptr, message.c_str(), sizeof(message.c_str()) * message.length());
}
