cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(kOferta)

if(NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE Release CACHE STRING 
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif()

if(NOT KOFERTA_VERSION )
  set(KOFERTA_VERSION "3.x-mannual" CACHE STRING
       "Please provide build version string"
       FORCE )
endif()
if(NOT CMAKE_BUILD_TYPE )
    set(BUILD_CUKE on CACHE BOOL "Build E2E test using cucumber" FORCE )
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Test)

add_subdirectory(MainWindow)

IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

add_executable(koferta ${GUI_TYPE} Main.cpp)
add_dependencies(koferta mainwindow)
set_property(TARGET koferta PROPERTY CXX_STANDARD 11)
#qt5_use_modules(koferta Widgets Sql Network PrintSupport)
target_link_libraries(koferta Qt5::Widgets mainwindow)

#
# Cucumber
#
if(BUILD_CUKE)
    set(CUKE_BOOST_LIBS thread system regex date_time program_options)
    find_package(Boost COMPONENTS ${CUKE_BOOST_LIBS})
    
    if(Boost_FOUND)
        include(ExternalProject)
        set(CUCUMBER_ROOT "${CMAKE_CURRENT_BINARY_DIR}/cucumber")
        set(CUCUMBER_LIBRARY "${CUCUMBER_ROOT}/src/cucumber-cpp-build/src/libcucumber-cpp.a")
        ExternalProject_Add(
            cucumber-cpp 
            GIT_REPOSITORY https://github.com/konserw/cucumber-cpp.git
            GIT_TAG ultimatekoferta
            PREFIX ${CUCUMBER_ROOT}
            CMAKE_ARGS -DCUKE_DISABLE_BOOST_TEST:bool=on -DCUKE_DISABLE_GTEST:bool=on -DCUKE_DISABLE_E2E_TESTS:bool=on -DCUKE_DISABLE_UNIT_TESTS:bool=on -DCUKE_ENABLE_EXAMPLES:bool=off -DGMOCK_VER:string=1.7.0
            INSTALL_COMMAND ""
            LOG_DOWNLOAD ON
            LOG_CONFIGURE ON
            LOG_BUILD ON
            BUILD_BYPRODUCTS "${CUCUMBER_LIBRARY}"
       #     BUILD_BYPRODUCTS "${CUCUMBER_MAIN_LIBRARY}"
        )
    
        add_library(libcucumber-cpp IMPORTED STATIC GLOBAL)
        add_dependencies(libcucumber-cpp cucumber-cpp)
        ExternalProject_Get_Property(cucumber-cpp source_dir binary_dir)
    
        set(CUCUMBER_INCLUDE_DIR ${source_dir}/include)
        include_directories(${CUCUMBER_INCLUDE_DIR})
        include_directories(MainWindow)
    
        set(CUKE_FEATURES features/e2e.feature features/initialization.feature)
        set(CUKE_LIBRARIES ${CUCUMBER_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
        add_executable(kofertaSteps features/step_definitions/kofertaSteps ${CUKE_FEATURES})
        set_property(TARGET kofertaSteps PROPERTY CXX_STANDARD 11)
        target_link_libraries(kofertaSteps mainwindow ${QT_LIBRARIES} ${CUKE_LIBRARIES})
    endif()
endif()
