set(TEST_HEADER_LIST
        tests.h
        )

set(TEST_SOURCE_LIST
        main.c
        )

include_directories(${CGREEN_PUBLIC_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})
add_executable(libdc_application_test ${TEST_SOURCE_LIST} ${TEST_HEADER_LIST} ${SOURCE_LIST} ${HEADER_LIST})

target_compile_features(libdc_application_test PRIVATE c_std_17)

target_include_directories(libdc_application_test PRIVATE ../include)
target_include_directories(libdc_application_test PRIVATE /usr/local/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(libdc_application_test PRIVATE /opt/homebrew/include)
else ()
    target_include_directories(libdc_application_test PRIVATE /usr/include)
endif ()

find_library(LIBCGREEN cgreen REQUIRED)
find_library(LIBDC_CONFIG config REQUIRED)
find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_ENV dc_env REQUIRED)
find_library(LIBDC_C dc_c REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UNIX dc_unix REQUIRED)
find_library(LIBDC_UTIL dc_util REQUIRED)
find_library(LIBDC_FSM dc_fsm REQUIRED)
find_library(LIBDC_FSM dc_application REQUIRED)

target_link_libraries(libdc_application_test PRIVATE ${LIBCGREEN})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_CONFIG})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_ERROR})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_ENV})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_C})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_POSIX})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_UNIX})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_UTIL})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_FSM})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_APPLICATION})

add_test(NAME libdc_application_test COMMAND libdc_application_test)

