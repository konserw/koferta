function(qt_add_test TEST_FILE)
        get_filename_component(TEST_NAME ${TEST_FILE} NAME)
        message(STATUS "Adding " ${TEST_NAME})
        add_executable(${TEST_NAME} ${TEST_FILE}.cpp)
        target_link_libraries(${TEST_NAME} PRIVATE ${ARGN} Qt5::Test)
        add_custom_target(make-directory-${TEST_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory ../test-results/${TEST_NAME})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} -o -,txt -o ../test-results/${TEST_NAME}/result.xml,xunitxml)
endfunction()

add_custom_target(make-directory-results ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ../test-results)
# TODO Compile tests with the least possible code, not with the entire library
qt_add_test(DatabaseTest mainwindow)

