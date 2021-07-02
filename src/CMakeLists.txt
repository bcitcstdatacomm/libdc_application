# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

set(HEADER_LIST
        "${libdc_application_SOURCE_DIR}/include/dc_application/application.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/settings.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/command_line.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/config.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/defaults.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/environment.h"
        "${libdc_application_SOURCE_DIR}/include/dc_application/options.h"
)

set(SOURCE_LIST
        "application.c"
        "settings.c"
        "command_line.c"
        "config.c"
        "defaults.c"
        "environment.c"
        "options.c"
)

# Make an static library
add_library(dc_application SHARED ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(dc_application PRIVATE ../include/dc_application)
target_include_directories(dc_application PRIVATE /usr/local/include)
target_link_directories(dc_application PRIVATE /usr/local/lib)

find_library(LIBDC_POSIX dc_posix REQUIRED)
if(LIBDC_POSIX)
    target_link_libraries(dc_application ${LIBDC_POSIX})
endif()

find_library(LIBDC_UTIL dc_util REQUIRED)
if(LIBDC_UTIL)
    target_link_libraries(dc_application ${LIBDC_UTIL})
endif()

find_library(LIBDC_FSM dc_fsm REQUIRED)
if(LIBDC_FSM)
    target_link_libraries(dc_application ${LIBDC_FSM})
endif()

find_library(LIBCONFIG config REQUIRED)
if(LIBCONFIG)
    target_link_libraries(dc_application ${LIBCONFIG})
endif()

# All users of this library will need at least C11
target_compile_features(dc_application PUBLIC c_std_11)
target_compile_options(dc_application PRIVATE -g)
target_compile_options(dc_application PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(dc_application PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(dc_application PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wpadded -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings )

install(TARGETS dc_application LIBRARY DESTINATION lib)
install(FILES ${HEADER_LIST} DESTINATION include/dc_application)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)
