#include "general/ArgumentParser/argument_parser.h"
#include "general/SharedMemory/memory_info.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <ostream>
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

  uint shmFileDescriptor0 = shm_open("Test0", O_CREAT | O_RDWR, 0666);
  ftruncate(shmFileDescriptor0, sizeof(MemoryInfo));
  std::cout << sizeof(MemoryInfo) << std::endl;

  MemoryInfo *intptr = (MemoryInfo *)mmap(NULL, sizeof(MemoryInfo), PROT_WRITE,
                                          MAP_SHARED, shmFileDescriptor0, 0);

  const int size = 2000000;
  uint shmFileDescriptor = shm_open("Test", O_CREAT | O_RDWR, 0666);
  ftruncate(shmFileDescriptor, size);
  std::ifstream ifs;
  char *message;
  std::streamsize sizeoffile;

  ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    ifs.open("/home/palnit/Desktop/ElteCarData/parkolo_pictures/"
             "Dev0_Image_w1920_h1200_fn1.jpg",
             std::ios::binary | std::ios::in);
    ifs.ignore(std::numeric_limits<std::streamsize>::max());
    sizeoffile = ifs.gcount();
    ifs.clear(); //  Since ignore will have set eof.
    ifs.seekg(0, std::ios_base::beg);
    message = new char[sizeoffile];
    ifs.read(message, sizeoffile - 1);
    ifs.close();
  } catch (std::ifstream::failure e) {
    std::cout << "Error" << std::endl;
  }
  std::cout << sizeoffile;

  void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shmFileDescriptor, 0);
  intptr->bufferSize = sizeoffile;
  std::memcpy(ptr, message, sizeoffile);
}
