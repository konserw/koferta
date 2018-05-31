cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(kOferta)

set(KOFERTA_SQL_USER CACHE STRING "SQL Database user" )
set(KOFERTA_SQL_PWD  CACHE STRING "SQL Database Password" )
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." )
set(KOFERTA_VERSION        "4.x-mannual" CACHE STRING "Please provide build version string" )
option(BUILD_CUKE 	       "Build E2E test using cucumber" OFF)
option(DISABLE_UNIT_TESTS  "Disable unit tests" OFF)
option(VALGRIND_TESTS      "Run tests within Valgrind" OFF)

set(GMOCK_VER "1.8.0")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Test)

add_subdirectory(MainWindow)
add_subdirectory(Logger)

IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(koferta ${GUI_TYPE} Main.cpp koferta.rc)
target_link_libraries(koferta Qt5::Widgets mainwindow logger)

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
        add_dependencies(kofertaSteps cucumber-cpp)
        target_link_libraries(kofertaSteps mainwindow ${QT_LIBRARIES} ${CUKE_LIBRARIES})
    endif()
endif()
