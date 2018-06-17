cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(kOferta)

set(KOFERTA_SQL_USER CACHE STRING "SQL Database user" )
set(KOFERTA_SQL_PWD  CACHE STRING "SQL Database Password" )
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." )
set(KOFERTA_VERSION     "4.x-mannual" CACHE STRING "Please provide build version string" )
option(BUILD_CUKE 	    "Build E2E test using cucumber" OFF)
option(UNIT_TESTS       "Enable unit tests" ON)
option(VALGRIND_TESTS   "Run tests within Valgrind" OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/forms) 
SET(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(KOFERTA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TS_FILES
    ${PROJECT_SOURCE_DIR}/translations/kOferta_pl.ts
    ${PROJECT_SOURCE_DIR}/translations/kOferta_en.ts
    ${PROJECT_SOURCE_DIR}/translations/kOferta_de.ts
)
#qt5_create_translation(qm_files ${CMAKE_SOURCE_DIR}/src/ ${TS_FILES})# OPTIONS ...)
qt5_add_translation(QM_FILES ${TS_FILES})
#set_source_files_properties(${qm_files} PROPERTIES GENERATED TRUE)
ADD_CUSTOM_TARGET (translations ALL DEPENDS ${QM_FILES})

#message(STATUS "Adding UI files to project")
#file(GLOB_RECURSE UI_SOURCES "forms/*.ui")
#qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})
#set_source_files_properties(${UI_GENERATED_HEADERS} PROPERTIES GENERATED TRUE)

set(KOFERTA_RESOURCES 
    ${PROJECT_SOURCE_DIR}/resources/mainwindow.qrc
    ${PROJECT_SOURCE_DIR}/resources/icon.rc
)

add_subdirectory(src)
add_subdirectory(Logger)

#
# Colored Terminal Output
#

if(UNIX AND (
       (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9))
    AND CMAKE_GENERATOR STREQUAL "Ninja")
  # These compilers generate coloured output, but by default only when their output channel is a
  # terminal (TTY/PTY). Ninja however captures all output in a pipe (per-subprocess), disabling
  # coloured compiler diagnostics. We forcefully enable it because Ninja, since 1.0.0
  # (ninja-build/ninja#198) takes care to strip VT100 CSI control sequences from the output if Ninja
  # itself is writing to a pipe instead of a terminal. As a result this should give us the best of
  # both worlds: coloured output when we're running in a terminal, plain output for editors, IDEs,
  # etc.
  set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always ${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)
endif()

#
# Valgrind
#

if(VALGRIND_TESTS)
    find_package(Valgrind REQUIRED)
    set(VALGRIND_ARGS --error-exitcode=2 --leak-check=full --undef-value-errors=no)
    if(NOT VALGRIND_VERSION_STRING VERSION_LESS 3.9)
        # Valgrind 3.9 no longer shows all leaks unless asked to
        list(APPEND VALGRIND_ARGS --show-leak-kinds=all)
    endif()
    function(add_test name)
        if(NOT name STREQUAL "NAME")
            _add_test(${VALGRIND_EXECUTABLE} ${VALGRIND_ARGS} ${ARGV})
            return()
        endif()

        set(TEST_ARGS ${ARGV})
        list(FIND TEST_ARGS COMMAND COMMAND_IDX)
        if(COMMAND_IDX EQUAL -1)
            message(AUTHOR_WARNING "Weird command-line given to add_test(), not injecting valgrind")
            _add_test(${ARGV})
            return()
        endif()

        # We want to operate on the COMMAND, not the 'COMMAND' keyword preceding it
        math(EXPR COMMAND_IDX "${COMMAND_IDX} + 1")

        # Keep add_test() behaviour of replacing COMMANDs, when executable targets, with their output files
        list(GET TEST_ARGS ${COMMAND_IDX} COMMAND)
        if(TARGET ${COMMAND})
            get_target_property(COMMAND_TYPE ${COMMAND} TYPE)
            if(COMMAND_TYPE STREQUAL "EXECUTABLE")
                # Inserting first, removing the original only after that, because inserting to the end of the list doesn't work
                math(EXPR ORIG_COMMAND_IDX "${COMMAND_IDX} + 1")
                list(INSERT TEST_ARGS ${COMMAND_IDX} "$<TARGET_FILE:${COMMAND}>")
                list(REMOVE_AT TEST_ARGS ${ORIG_COMMAND_IDX})
            endif()
        endif()

        # Insert the valgrind command line, before the command to execute
        list(INSERT TEST_ARGS ${COMMAND_IDX} ${VALGRIND_EXECUTABLE} ${VALGRIND_ARGS})

        _add_test(${TEST_ARGS})
    endfunction()
endif()

#
# Tests
#

if(UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

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
        target_link_libraries(kofertaSteps mainwindow Qt5::Core Qt5::Widgets Qt5::Test ${CUKE_LIBRARIES})
    endif()
endif()
