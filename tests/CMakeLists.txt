set(HEADERS
)

set(SOURCES
tests-main.cpp
tests-clientsConnectToTheServer.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_executable(tests ${PROJECT_CONTENT})

set(LIBRARIES
Catch2::Catch2
engine-lib 
)

target_link_libraries(tests ${LIBRARIES})
