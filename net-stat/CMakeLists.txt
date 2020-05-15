set(HEADERS
)

set(SOURCES
Program.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_executable(net-stat ${PROJECT_CONTENT})

set(LIBRARIES
snmp-client
engine-lib 
${Boost_LIBRARIES}
)

target_link_libraries(net-stat PRIVATE ${LIBRARIES})

target_include_directories(net-stat PRIVATE ${Boost_INCLUDE_DIRS})
