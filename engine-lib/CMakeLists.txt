add_library(engine-lib STATIC Server.cpp Server.h Client.cpp Client.h)

target_include_directories(engine-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
                          