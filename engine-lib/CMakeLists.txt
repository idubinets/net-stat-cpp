add_library(engine-lib STATIC Server.cpp Client.cpp)

target_include_directories(engine-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
                          