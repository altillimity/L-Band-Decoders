cmake_minimum_required (VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (QPSK-Demodulator)
file(GLOB_RECURSE QPSK-Demodulator_CPPS src/*.cpp)

add_executable(QPSK-Demodulator ${QPSK-Demodulator_CPPS})

target_include_directories(QPSK-Demodulator PUBLIC src)
target_link_libraries(QPSK-Demodulator PUBLIC m)
