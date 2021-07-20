/*
 * Copyright 2021-2021 D'Arcy Smith.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "application.h"
#include <dc_fsm/fsm.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern char **environ;

static int    create_settings(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    parse_command_line(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    read_env_vars(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    read_config(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    set_defaults(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    run(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    cleanup(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    destroy_settings(const struct dc_posix_env *env, struct dc_error *err, void *arg);

static int    create_settings_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    parse_command_line_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    read_env_vars_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    read_config_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    set_defaults_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    run_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    cleanup_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);
static int    destroy_settings_error(const struct dc_posix_env *env, struct dc_error *err, void *arg);

struct dc_application_lifecycle
{
    struct dc_application_settings *(*create_settings)(const struct dc_posix_env *env, struct dc_error *err);
    int (*parse_command_line)(const struct dc_posix_env *env,
                              struct dc_error *          err,
                              struct dc_application_settings *,
                              int   argc,
                              char *argv[]);
    int (*read_env_vars)(const struct dc_posix_env *env,
                         struct dc_error *          err,
                         struct dc_application_settings *,
                         char **envvars);
    int (*read_config)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *);
    int (*set_defaults)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *);
    int (*run)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *);
    int (*cleanup)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *);
    int (*destroy_settings)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings **);
};

struct dc_application_info
{
    char *                           name;
    FILE *                           verbose_file;
    struct dc_application_lifecycle *lifecycle;
    struct dc_application_settings * settings;
    int                              argc;
    char *                           default_config_path;
    char **                          argv;
};

enum application_states
{
    CREATE_SETTINGS = DC_FSM_USER_START,    // 2
    PARSE_COMMAND_LINE,                     // 3
    READ_ENV_VARS,                          // 4
    READ_CONFIG,                            // 5
    SET_DEFAULTS,                           // 6
    RUN,                                    // 7
    CLEANUP,                                // 8
    DESTROY_SETTINGS,                       // 9
    CREATE_SETTINGS_ERROR,                  // 10
    PARSE_COMMAND_LINE_ERROR,               // 11
    READ_ENV_VARS_ERROR,                    // 12
    READ_CONFIG_ERROR,                      // 13
    SET_DEFAULTS_ERROR,                     // 14
    RUN_ERROR,                              // 15
    CLEANUP_ERROR,                          // 16
    DESTROY_SETTINGS_ERROR,                 // 17
};

struct dc_application_lifecycle *dc_application_lifecycle_create(
    const struct dc_posix_env *env,
    struct dc_error *          err,
    struct dc_application_settings *(*create_settings_func)(const struct dc_posix_env *env, struct dc_error *err),
    int (*destroy_settings_func)(const struct dc_posix_env *env,
                                 struct dc_error *          err,
                                 struct dc_application_settings **),
    int (*run_func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *))
{
    struct dc_application_lifecycle *lifecycle;

    DC_TRACE(env);
    lifecycle = dc_calloc(env, err, 1, sizeof(struct dc_application_lifecycle));

    if(dc_error_has_no_error(err))
    {
        lifecycle->create_settings  = create_settings_func;
        lifecycle->run              = run_func;
        lifecycle->destroy_settings = destroy_settings_func;
    }

    return lifecycle;
}

void dc_application_lifecycle_destroy(const struct dc_posix_env *env, struct dc_application_lifecycle **plifecycle)
{
    DC_TRACE(env);
    dc_free(env, *plifecycle, sizeof(struct dc_application_lifecycle));
    *plifecycle = NULL;
}

void dc_application_lifecycle_set_parse_command_line(
    const struct dc_posix_env *      env,
    struct dc_application_lifecycle *lifecycle,
    int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *, int, char *[]))
{
    DC_TRACE(env);
    lifecycle->parse_command_line = func;
}

void dc_application_lifecycle_set_read_env_vars(
    const struct dc_posix_env *      env,
    struct dc_application_lifecycle *lifecycle,
    int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *, char **))
{
    DC_TRACE(env);
    lifecycle->read_env_vars = func;
}

void dc_application_lifecycle_set_read_config(const struct dc_posix_env *      env,
                                              struct dc_application_lifecycle *lifecycle,
                                              int (*func)(const struct dc_posix_env *env,
                                                          struct dc_error *          err,
                                                          struct dc_application_settings *))
{
    DC_TRACE(env);
    lifecycle->read_config = func;
}

void dc_application_lifecycle_set_set_defaults(const struct dc_posix_env *      env,
                                               struct dc_application_lifecycle *lifecycle,
                                               int (*func)(const struct dc_posix_env *env,
                                                           struct dc_error *          err,
                                                           struct dc_application_settings *))
{
    DC_TRACE(env);
    lifecycle->set_defaults = func;
}

void dc_application_lifecycle_set_cleanup(const struct dc_posix_env *      env,
                                          struct dc_application_lifecycle *lifecycle,
                                          int (*func)(const struct dc_posix_env *env,
                                                      struct dc_error *          err,
                                                      struct dc_application_settings *))
{
    DC_TRACE(env);
    lifecycle->cleanup = func;
}

struct dc_application_info *
dc_application_info_create(const struct dc_posix_env *env, struct dc_error *err, const char *name, FILE *verbose_file)
{
    struct dc_application_info *info;

    DC_TRACE(env);
    info = dc_calloc(env, err, 1, sizeof(struct dc_application_info));

    if(dc_error_has_no_error(err))
    {
        info->verbose_file = verbose_file;
        info->name         = dc_malloc(env, err, dc_strlen(env, name) + 1);

        if(dc_error_has_no_error(err))
        {
            dc_strcpy(env, info->name, name);
        }
        else
        {
            dc_application_info_destroy(env, &info);
            info = NULL;
        }
    }

    return info;
}

void dc_application_info_destroy(const struct dc_posix_env *env, struct dc_application_info **pinfo)
{
    struct dc_application_info *info;

    DC_TRACE(env);
    info = *pinfo;

    if(info->default_config_path)
    {
        size_t length;

        length = dc_strlen(env, info->default_config_path);
        dc_free(env, info->default_config_path, length);
    }

    if(info->name)
    {
        size_t length;

        length = dc_strlen(env, info->name);
        dc_free(env, info->name, length);
    }

    dc_free(env, *pinfo, sizeof(struct dc_application_info));

    if(env->null_free)
    {
        *pinfo = NULL;
    }
}

int dc_application_run(const struct dc_posix_env * env,
                       struct dc_error *           err,
                       struct dc_application_info *info,
                       struct dc_application_lifecycle *(*create_lifecycle_func)(const struct dc_posix_env *env,
                                                                                 struct dc_error *          err),
                       void (*destroy_lifecycle_func)(const struct dc_posix_env *       env,
                                                      struct dc_application_lifecycle **plifecycle),
                       const char *default_config_path,
                       int         argc,
                       char *      argv[])
{
    int ret_val;

    DC_TRACE(env);
    ret_val         = -1;
    info->lifecycle = create_lifecycle_func(env, err);

    if(dc_error_has_no_error(err))
    {
        info->argc = argc;
        info->argv = argv;

        if(default_config_path)
        {
            info->default_config_path = dc_malloc(env, err, dc_strlen(env, default_config_path) + 1);

            if(dc_error_has_no_error(err))
            {
                dc_strcpy(env, info->default_config_path, default_config_path);
            }
        }

        if(dc_error_has_no_error(err))
        {
            struct dc_fsm_info *            fsm_info;
            static struct dc_fsm_transition transitions[] = {
                {DC_FSM_INIT, CREATE_SETTINGS, create_settings},
                {CREATE_SETTINGS, PARSE_COMMAND_LINE, parse_command_line},
                {PARSE_COMMAND_LINE, READ_ENV_VARS, read_env_vars},
                {READ_ENV_VARS, READ_CONFIG, read_config},
                {READ_CONFIG, SET_DEFAULTS, set_defaults},
                {SET_DEFAULTS, RUN, run},
                {RUN, CLEANUP, cleanup},
                {CLEANUP, DESTROY_SETTINGS, destroy_settings},
                {DESTROY_SETTINGS, DC_FSM_EXIT, NULL},
                {CREATE_SETTINGS, CREATE_SETTINGS_ERROR, create_settings_error},
                {PARSE_COMMAND_LINE, PARSE_COMMAND_LINE_ERROR, parse_command_line_error},
                {READ_ENV_VARS, READ_ENV_VARS_ERROR, read_env_vars_error},
                {READ_CONFIG, READ_CONFIG_ERROR, read_config_error},
                {SET_DEFAULTS, SET_DEFAULTS_ERROR, set_defaults_error},
                {RUN, RUN_ERROR, run_error},
                {CLEANUP, CLEANUP_ERROR, cleanup_error},
                {DESTROY_SETTINGS, DESTROY_SETTINGS_ERROR, destroy_settings_error},
                {CREATE_SETTINGS_ERROR, DC_FSM_EXIT, NULL},
                {PARSE_COMMAND_LINE_ERROR, DESTROY_SETTINGS, destroy_settings},
                {READ_ENV_VARS_ERROR, DESTROY_SETTINGS, destroy_settings},
                {READ_CONFIG_ERROR, DESTROY_SETTINGS, destroy_settings},
                {SET_DEFAULTS_ERROR, DESTROY_SETTINGS, destroy_settings},
                {RUN_ERROR, DESTROY_SETTINGS, destroy_settings},
                {CLEANUP_ERROR, DESTROY_SETTINGS, destroy_settings},
                {DESTROY_SETTINGS_ERROR, DC_FSM_EXIT, NULL},
                {DC_FSM_IGNORE, DC_FSM_IGNORE, NULL},
            };

            fsm_info = dc_fsm_info_create(env, err, info->name);

            if(dc_error_has_no_error(err))
            {
                int from_state;
                int to_state;

                ret_val = dc_fsm_run(env, err, fsm_info, &from_state, &to_state, info, transitions);
                dc_fsm_info_destroy(env, &fsm_info);
            }
        }

        destroy_lifecycle_func(env, &info->lifecycle);
    }

    return ret_val;
}

static int create_settings(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->create_settings)
    {
        info->settings = info->lifecycle->create_settings(env, err);

        if(dc_error_has_error(err))
        {
            ret_val = -1;
        }
    }
    else
    {
        info->settings = NULL;
    }

    if(ret_val == 0)
    {
        ret_val = PARSE_COMMAND_LINE;
    }
    else
    {
        ret_val = CREATE_SETTINGS_ERROR;
    }

    return ret_val;
}

static int parse_command_line(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->parse_command_line)
    {
        ret_val = info->lifecycle->parse_command_line(env, err, info->settings, info->argc, info->argv);
    }

    if(ret_val == 0)
    {
        ret_val = READ_ENV_VARS;
    }
    else
    {
        ret_val = PARSE_COMMAND_LINE_ERROR;
    }

    return ret_val;
}

static int read_env_vars(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->read_env_vars)
    {
        ret_val = info->lifecycle->read_env_vars(env, err, info->settings, environ);
    }

    if(ret_val == 0)
    {
        ret_val = READ_CONFIG;
    }
    else
    {
        ret_val = READ_ENV_VARS_ERROR;
    }

    return ret_val;
}

static int read_config(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->read_config && info->default_config_path)
    {
        // this will not overwrite if it was set earlier, no need for an "if" to check if it was set already
        dc_setting_path_set(env, err, info->settings->config_path, info->default_config_path, DC_SETTING_NONE);

        if(dc_error_has_no_error(err))
        {
            ret_val = info->lifecycle->read_config(env, err, info->settings);
        }
        else
        {
            ret_val = -1;
        }
    }

    if(ret_val == 0)
    {
        ret_val = SET_DEFAULTS;
    }
    else
    {
        ret_val = READ_CONFIG_ERROR;
    }

    return ret_val;
}

static int set_defaults(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->set_defaults)
    {
        ret_val = info->lifecycle->set_defaults(env, err, info->settings);
    }

    if(ret_val == 0)
    {
        ret_val = RUN;
    }
    else
    {
        ret_val = SET_DEFAULTS_ERROR;
    }

    return ret_val;
}

static int run(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = info->lifecycle->run(env, err, info->settings);

    if(ret_val == 0)
    {
        ret_val = CLEANUP;
    }
    else
    {
        ret_val = RUN_ERROR;
    }

    return ret_val;
}

static int cleanup(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->cleanup)
    {
        ret_val = info->lifecycle->cleanup(env, err, info->settings);
    }

    if(ret_val == 0)
    {
        ret_val = DESTROY_SETTINGS;
    }
    else
    {
        ret_val = CLEANUP_ERROR;
    }

    return ret_val;
}

static int destroy_settings(const struct dc_posix_env *env, struct dc_error *err, void *arg)
{
    struct dc_application_info *info;
    int                         ret_val;

    DC_TRACE(env);
    info    = arg;
    ret_val = 0;

    if(info->lifecycle->destroy_settings)
    {
        dc_setting_path_destroy(env, &info->settings->config_path);
        ret_val = info->lifecycle->destroy_settings(env, err, &info->settings);
    }

    if(ret_val == 0)
    {
        ret_val = DC_FSM_EXIT;
    }
    else
    {
        ret_val = DESTROY_SETTINGS_ERROR;
    }

    return ret_val;
}

static int create_settings_error(const struct dc_posix_env *              env,
                                 __attribute__((unused)) struct dc_error *err,
                                 __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DC_FSM_EXIT;
}

static int parse_command_line_error(const struct dc_posix_env *              env,
                                    __attribute__((unused)) struct dc_error *err,
                                    __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int read_env_vars_error(const struct dc_posix_env *              env,
                               __attribute__((unused)) struct dc_error *err,
                               __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int read_config_error(const struct dc_posix_env *              env,
                             __attribute__((unused)) struct dc_error *err,
                             __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int set_defaults_error(const struct dc_posix_env *              env,
                              __attribute__((unused)) struct dc_error *err,
                              __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int run_error(const struct dc_posix_env *              env,
                     __attribute__((unused)) struct dc_error *err,
                     __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int cleanup_error(const struct dc_posix_env *              env,
                         __attribute__((unused)) struct dc_error *err,
                         __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DESTROY_SETTINGS;
}

static int destroy_settings_error(const struct dc_posix_env *              env,
                                  __attribute__((unused)) struct dc_error *err,
                                  __attribute__((unused)) void *           arg)
{
    DC_TRACE(env);

    return DC_FSM_EXIT;
}
