find_package(Qt5Core REQUIRED)

add_definitions(-DVERSION="${KOFERTA_VERSION}")
add_definitions(-DEXPORT_LOGGER)
add_definitions(-DQT_MESSAGELOGCONTEXT)

add_library(logger SHARED Logger.cpp Logger.hpp)
#set_property(TARGET logger PROPERTY CXX_STANDARD 11)
target_link_libraries(logger Qt5::Core)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)
