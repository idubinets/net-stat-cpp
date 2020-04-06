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

