set(HEADERS
Client.h
Server.h
)

set(SOURCES
Client.cpp
Server.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_library(engine-lib STATIC ${PROJECT_CONTENT})

target_include_directories(engine-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
                          