set(TEST_NAME ${PROJECT_NAME}_test)
set(COVR_NAME ${PROJECT_NAME}_coverage)
set(DEMO_NAME ${PROJECT_NAME}_demo)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(INC_DIR ${PROJECT_SOURCE_DIR}/lib)
include_directories(${INC_DIR})

if(${CMAKE_BUILD_TYPE} MATCHES "Coverage")
        set(LIB_DIR ../lib)
        set(SRC_LIST ${LIB_DIR}/bplustree.c bplustree_coverage.c)
        add_executable(${COVR_NAME} ${SRC_LIST})
        set(CMAKE_C_FLAGS "-O2 -Wall --coverage")
        include(CodeCoverage)
        setup_target_for_coverage(coverage ${COVR_NAME} coverage)
else()
        set(SRC_LIST bplustree_demo.c)
        add_executable(${DEMO_NAME} ${SRC_LIST})
        set(CMAKE_C_FLAGS "-O2 -Wall -Werror -Wextra")
        target_link_libraries(${DEMO_NAME} ${LIB_BPLUSTREE_NAME})

        # Build the assertion-based functional test driver (the same sources
        # as the "Coverage" runner) and register it with CTest. It replays the
        # generated "testcase" file found in its working directory.
        set(LIB_DIR ../lib)
        set(TEST_SRC_LIST ${LIB_DIR}/bplustree.c bplustree_coverage.c)
        add_executable(${TEST_NAME} ${TEST_SRC_LIST})
        # bplustree.c triggers many -Wextra warnings, relax flags for this target
        target_compile_options(${TEST_NAME} PRIVATE -Wno-error -Wno-extra)
        add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
        set_tests_properties(${TEST_NAME} PROPERTIES
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

        # File-driven range / membership query test: the tree is built from
        # bplustree_build.conf, re-opened from disk and answered against
        # bplustree_search.conf (see bplustree_query_test.c).
        add_executable(bplustree_query_test
                ${LIB_DIR}/bplustree.c bplustree_query_test.c)
        target_compile_options(bplustree_query_test PRIVATE -Wno-error -Wno-extra)
        add_test(NAME bplustree_query_test
                 COMMAND $<TARGET_FILE:bplustree_query_test>
                         ${CMAKE_CURRENT_SOURCE_DIR}/bplustree_build.conf
                         ${CMAKE_CURRENT_SOURCE_DIR}/bplustree_search.conf)
endif()
