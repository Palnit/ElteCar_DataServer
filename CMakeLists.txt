cmake_minimum_required(VERSION 3.22)

project(DataServer VERSION 0.1 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(PROJECT_SOURCES
   src/main.cpp
   src/general/ArgumentParser/parser.cpp
   src/general/SharedMemory/bufferd_writer.cpp
   src/argument_handler.cpp
   src/cpp_file_handling.cpp
)

set(HEADER_FILES
    include/general/ArgumentParser/parser.h
    include/general/ArgumentParser/runner.h
    include/general/ArgumentParser/base_runner.h
    include/general/SharedMemory/info.h
    include/general/SharedMemory/bufferd_writer.h
    include/argument_handler.h
    include/cpp_file_handling.h
)


add_executable(DataServer
    ${PROJECT_SOURCES}
    ${HEADER_FILES}
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)


install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
