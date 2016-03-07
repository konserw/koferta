cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(kOferta)

if(NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE Release CACHE STRING 
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif()

find_package(Qt5Widgets REQUIRED)
add_subdirectory(MainWindow)

IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

add_executable(koferta ${GUI_TYPE} Main.cpp)
add_dependencies(koferta mainwindow)
#set_property(TARGET koferta PROPERTY CXX_STANDARD 11)
#qt5_use_modules(koferta Widgets Sql Network PrintSupport)
target_link_libraries(koferta Qt5::Widgets mainwindow)
