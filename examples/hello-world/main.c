#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


static int run(const struct dc_env *env, struct dc_error *err, struct dc_application_settings *settings);
static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_env *env,
                          const char *file_name,
                          const char *function_name,
                          size_t line_number);


int main(int argc, char *argv[])
{
    dc_posix_tracer tracer;
    dc_error_reporter reporter;
    struct dc_env env;
    struct dc_error err;
    struct dc_application_info *info;
    int ret_val;

    reporter = error_reporter;
    tracer = NULL;
//    tracer = trace_reporter;
    dc_error_init(&err, reporter);
    dc_env_init(&env, tracer);
    info = dc_application_info_create(&env, &err, "Hello World Application");
    ret_val = dc_application_run(&env, &err, info, NULL, NULL, run, dc_default_create_lifecycle, dc_default_destroy_lifecycle, NULL, argc, argv);
    dc_application_info_destroy(&env, &info);
    dc_error_reset(&err);

    return ret_val;
}

static int run(const struct dc_env *env, struct dc_error *err, struct dc_application_settings *settings)
{
    DC_TRACE(env);

    printf("Hello, World!\n");

    return EXIT_SUCCESS;
}

static void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "ERROR: %s : %s : @ %zu : %d\n", err->file_name, err->function_name, err->line_number, 0);
    fprintf(stderr, "ERROR: %s\n", err->message);
}

static void trace_reporter(const struct dc_env *env,
                          const char *file_name,
                          const char *function_name,
                          size_t line_number)
{
    fprintf(stdout, "TRACE: %s : %s : @ %zu\n", file_name, function_name, line_number);
}
