cmake_minimum_required(VERSION 3.22)

project(DataServer VERSION 0.1 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(PROJECT_SOURCES
   src/main.cpp
   src/general/ArgumentParser/argument_parser.cpp
   src/general/ArgumentParser/base_runner.cpp
)

set(HEADER_FILES
    include/general/ArgumentParser/argument_parser.h
    include/general/ArgumentParser/argument_runner.h
    include/general/ArgumentParser/base_runner.h
    include/general/SharedMemory/memory_info.h
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
