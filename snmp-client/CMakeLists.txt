set(HEADERS
SNMPClient.h
)

set(SOURCES
SNMPClient.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_library(snmp-client STATIC ${PROJECT_CONTENT})

target_include_directories(snmp-client INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${NetSNMPIncludeDir} ${Boost_INCLUDE_DIR})

set(LIBRARIES
${NetSNMP}
)

target_link_libraries(snmp-client PRIVATE ${LIBRARIES})
