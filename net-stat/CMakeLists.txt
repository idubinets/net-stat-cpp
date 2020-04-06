set(HEADERS
)

set(SOURCES
Program.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_executable(net-stat ${PROJECT_CONTENT})

target_link_libraries(net-stat PRIVATE engine-lib)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED QUIET COMPONENTS 
program_options
)
target_link_libraries(net-stat PRIVATE ${Boost_LIBRARIES})
target_include_directories(net-stat PRIVATE ${Boost_INCLUDE_DIRS})
