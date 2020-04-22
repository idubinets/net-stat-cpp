set(tests_DEFINITIONS
-DBOOST_DATE_TIME_NO_LIB
-DBOOST_REGEX_NO_LIB
)
add_definitions(${tests_DEFINITIONS})

set(HEADERS
)

set(SOURCES
tests-main.cpp
tests-clientsConnectToServer.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_executable(tests ${PROJECT_CONTENT})

find_package(Catch2 REQUIRED)

set(LIBRARIES
Catch2::Catch2
engine-lib 
)

target_link_libraries(tests  ${LIBRARIES})
