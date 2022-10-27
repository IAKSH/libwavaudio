cmake_minimum_required(VERSION 3.0.0)
project(wavaudio VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(wavaudio wavaudio.cpp wavaudio.hpp)
add_executable(waveaudio_test ${CMAKE_CURRENT_LIST_DIR}/test/test.cpp)
target_link_libraries(waveaudio_test PUBLIC wavaudio openal)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
