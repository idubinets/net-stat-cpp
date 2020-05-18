cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BoostDefenitions
-DBOOST_DATE_TIME_NO_LIB
-DBOOST_REGEX_NO_LIB
)
add_definitions(${BoostDefenitions})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS 
program_options
thread
)

find_package(Catch2 REQUIRED)

find_path(NetSNMPIncludeDir "net-snmp")

find_library(NetSNMP "netsnmp")

add_subdirectory(tests)
add_subdirectory(engine-lib)
add_subdirectory(snmp-client)
add_subdirectory(net-stat)
