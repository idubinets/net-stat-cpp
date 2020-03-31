cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0)

set(CMAKE_CPP_STANDARD 11)
set(CMAKE_CPP_STANDARD_REQUIRED True)

add_executable(MyProject net-stat/Program.cpp)
add_library(Server engine-lib/Server.cpp)
add_library(Client engine-lib/Client.cpp)
target_link_libraries(MyProject PUBLIC Server)
target_link_libraries(MyProject PUBLIC Client)

target_include_directories(MyProject PUBLIC
                          "${PROJECT_SOURCE_DIR}/engine-lib"
                          )