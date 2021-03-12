cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project(entis)

option(TEST "Build tests" ON)

add_subdirectory(${PROJECT_NAME})

if(TEST)
    enable_testing()
    add_subdirectory(test)
    add_test(entis_tests entis_tests)
endif()
