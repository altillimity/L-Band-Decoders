cmake_minimum_required (VERSION 3.0.0)

project (MetOp-Decoder)

file(GLOB_RECURSE MetOp-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(MetOp-Decoder ${MetOp-Decoder_CPPS})

target_include_directories(MetOp-Decoder PUBLIC src)

target_link_libraries(MetOp-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-Decoder PUBLIC Threads::Threads)

target_link_libraries (MetOp-Decoder PUBLIC "correct")

install(TARGETS MetOp-Decoder DESTINATION bin)