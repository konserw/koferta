if(GMOCK_FOUND)
    function(g_add_test TEST_FILE)
        get_filename_component(TEST_NAME ${TEST_FILE} NAME)
        message(STATUS "Adding " ${TEST_NAME})
        add_executable(${TEST_NAME} ${TEST_FILE}.cpp)
        target_link_libraries(${TEST_NAME} MainWindow ${ARGN} ${GMOCK_LIBRARIES})
        gtest_add_tests(${TEST_NAME} "" ${TEST_FILE}.cpp)
        # Run all tests in executable at once too. This ensures that the used fixtures get tested
        # properly too. Additionally gather the output in jUnit compatible output for CI.
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} "--gtest_output=xml:TEST-${TEST_NAME}.xml")
    endfunction()

    # TODO Compile tests with the least possible code, not with the entire library
#    g_add_test()

endif()
