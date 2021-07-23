add_compile_definitions(_POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)

if (APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
endif ()

set(TEST_HEADER_LIST
        tests.h
        )

set(TEST_SOURCE_LIST
        main.c
        )

include_directories(${CGREEN_PUBLIC_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})
add_executable(libdc_application_test
        ${TEST_SOURCE_LIST} ${TEST_HEADER_LIST} ${SOURCE_LIST} ${HEADER_LIST})

target_compile_features(libdc_application_test PRIVATE c_std_11)
target_compile_options(libdc_application_test PRIVATE -iquote "${libdc_error_SOURCE_DIR}/include/dc_error")
target_compile_options(libdc_application_test PRIVATE -g)
target_compile_options(libdc_application_test PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(libdc_application_test PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(libdc_application_test PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)

target_include_directories(libdc_application_test PRIVATE ../include/dc_application)
target_include_directories(libdc_application_test PRIVATE /usr/include)
target_include_directories(libdc_application_test PRIVATE /usr/local/include)

find_library(LIBCGREEN   cgreen REQUIRED)
find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UTIL  dc_util REQUIRED)
find_library(LIBDC_FSM   dc_fsm REQUIRED)
find_library(LIBCONFIG   config REQUIRED)
target_link_libraries(libdc_application_test PRIVATE ${LIBCGREEN})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_ERROR})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_POSIX})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_UTIL})
target_link_libraries(libdc_application_test PRIVATE ${LIBDC_FSM})
target_link_libraries(libdc_application_test PRIVATE ${LIBCONFIG})

add_test(NAME dc_application_test COMMAND libdc_application_test)