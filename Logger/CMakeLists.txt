find_package(Qt5Core REQUIRED)

add_definitions(-DVERSION="${KOFERTA_VERSION}")
add_definitions(-DEXPORT_LOGGER)
add_definitions(-DQT_MESSAGELOGCONTEXT)

add_library(logger SHARED Logger.cpp Logger.h)
set_property(TARGET logger PROPERTY CXX_STANDARD 11)
target_link_libraries(logger Qt5::Core)
