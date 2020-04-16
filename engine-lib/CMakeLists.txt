set(engine-lib_DEFINITIONS
-DBOOST_DATE_TIME_NO_LIB
-DBOOST_REGEX_NO_LIB
)
add_definitions(${engine-lib_DEFINITIONS})

set(HEADERS
Client.h
Server.h
TMessage.h
)

set(SOURCES
Client.cpp
Server.cpp
)

set(PROJECT_CONTENT
${SOURCES} ${HEADERS}
)

add_library(engine-lib STATIC ${PROJECT_CONTENT})

find_package(Boost REQUIRED)

target_include_directories(engine-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${Boost_INCLUDE_DIR})
                          