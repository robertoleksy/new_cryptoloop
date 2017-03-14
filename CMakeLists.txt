project(new_cryptoloop)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE libraries "./src/*.hpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -g3 -std=c++14 -Wall -pedantic")

add_executable(${PROJECT_NAME} ${libraries} "src/main.cpp")
target_link_libraries(${PROJECT_NAME} sodium pthread)
