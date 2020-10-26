cmake_minimum_required (VERSION 3.0.0)

project (CADU-RSynchroderand)
file(GLOB_RECURSE CADU-RSynchroderand_CPPS src/*.cpp)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_executable(CADU-RSynchroderand ${CADU-RSynchroderand_CPPS})

target_include_directories(CADU-RSynchroderand PUBLIC src)

target_link_libraries(CADU-RSynchroderand PUBLIC m)

target_link_libraries (CADU-RSynchroderand PUBLIC "correct")

install(TARGETS CADU-RSynchroderand DESTINATION bin)
