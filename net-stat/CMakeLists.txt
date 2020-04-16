set(net-stat_DEFINITIONS
-DBOOST_DATE_TIME_NO_LIB
-DBOOST_REGEX_NO_LIB
)
add_definitions(${net-stat_DEFINITIONS})

set(HEADERS
)

set(SOURCES
Program.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_executable(net-stat ${PROJECT_CONTENT})

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS 
program_options
thread
)

set(LIBRARIES
engine-lib 
${Boost_LIBRARIES}
)

target_link_libraries(net-stat PRIVATE ${LIBRARIES})

target_include_directories(net-stat PRIVATE ${Boost_INCLUDE_DIRS})
