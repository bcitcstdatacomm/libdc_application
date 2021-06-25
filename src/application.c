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
#include <stdlib.h>
#include <string.h>
#include <dc_fsm/fsm.h>


extern char **environ;


static int create_settings(void *arg);
static int parse_command_line(void *arg);
static int read_env_vars(void *arg);
static int read_config(void *arg);
static int set_defaults(void *arg);
static int run(void *arg);
static int cleanup(void *arg);
static int destroy_settings(void *arg);
static int error(void *arg);

struct dc_application_lifecycle
{
    struct dc_application_settings *(*create_settings)(void);
    int (*parse_command_line)(struct dc_application_settings *, int argc, char *argv[]);
    int (*read_env_vars)(struct dc_application_settings *, char **env);
    int (*read_config)(struct dc_application_settings *);
    int (*set_defaults)(struct dc_application_settings *);
    int (*run)(struct dc_application_settings *);
    int (*cleanup)(struct dc_application_settings *);
    int (*destroy_settings)(struct dc_application_settings **);
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
struct dc_application_info
{
    char *name;
    FILE *verbose_file;
    struct dc_application_lifecycle *lifecycle;
    struct dc_application_settings *settings;
    int argc;
    char *default_config_path;
    char **argv;
};
#pragma GCC diagnostic pop


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
    ERROR,                                  // 10
};


struct dc_application_lifecycle *dc_application_lifecycle_create(struct dc_application_settings *(*create_settings_func)(void), int (*destroy_settings_func)(struct dc_application_settings **), int (*run_func)(struct dc_application_settings *))
{
    struct dc_application_lifecycle *lifecycle;

    lifecycle = calloc(1, sizeof(struct dc_application_lifecycle));
    lifecycle->create_settings = create_settings_func;
    lifecycle->run = run_func;
    lifecycle->destroy_settings = destroy_settings_func;

    return lifecycle;
}


void dc_application_lifecycle_destroy(struct dc_application_lifecycle **plifecycle)
{
    memset(*plifecycle, 0, sizeof(struct dc_application_lifecycle));
    free(*plifecycle);
    *plifecycle = NULL;
}

void dc_application_lifecycle_set_parse_command_line(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *, int, char *[]))
{
    lifecycle->parse_command_line = func;
}

void dc_application_lifecycle_set_read_env_vars(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *, char **))
{
    lifecycle->read_env_vars = func;
}

void dc_application_lifecycle_set_read_config(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *))
{
    lifecycle->read_config = func;
}

void dc_application_lifecycle_set_set_defaults(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *))
{
    lifecycle->set_defaults = func;
}

void dc_application_lifecycle_set_cleanup(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *))
{
    lifecycle->cleanup = func;
}

struct dc_application_info *dc_application_info_create(const char *name,
                                                       FILE *verbose_file)
{
    struct dc_application_info *info;

    info = calloc(1, sizeof(struct dc_application_info));
    info->verbose_file = verbose_file;
    info->name         = malloc(strlen(name) + 1);
    strcpy(info->name, name);

    return info;
}


void dc_application_info_destroy(struct dc_application_info **pinfo)
{
    memset((*pinfo)->default_config_path, '\0', strlen((*pinfo)->default_config_path));
    free((*pinfo)->default_config_path);
    memset((*pinfo)->name, '\0', strlen((*pinfo)->name));
    free((*pinfo)->name);
    memset(*pinfo, 0, sizeof(struct dc_application_info));
    free(*pinfo);
    *pinfo = NULL;
}

int dc_application_run(struct dc_application_info *info,
                       struct dc_application_lifecycle *(*create_lifecycle_func)(void),
                       const char *default_config_path,
                       int argc,
                       char *argv[])
{
    struct dc_fsm_info *fsm_info;
    struct dc_fsm_transition transitions[] =
            {
                    { DC_FSM_INIT,        CREATE_SETTINGS,    create_settings    },
                    { CREATE_SETTINGS,    PARSE_COMMAND_LINE, parse_command_line },
                    { PARSE_COMMAND_LINE, READ_ENV_VARS,      read_env_vars      },
                    { READ_ENV_VARS,      READ_CONFIG,        read_config        },
                    { READ_CONFIG,        SET_DEFAULTS,       set_defaults       },
                    { SET_DEFAULTS,       RUN,                run                },
                    { RUN,                CLEANUP,            cleanup            },
                    { RUN,                ERROR,              error              },
                    { ERROR,              CLEANUP,            cleanup            },
                    { CLEANUP,            DESTROY_SETTINGS,   destroy_settings   },
                    { DESTROY_SETTINGS,   DC_FSM_EXIT,        NULL               },
                    { DC_FSM_IGNORE,      DC_FSM_IGNORE,      NULL               },
            };
    int from_state;
    int to_state;
    int ret_val;

    info->lifecycle = create_lifecycle_func();
    info->argc      = argc;
    info->argv      = argv;

    if(default_config_path)
    {
        info->default_config_path = malloc(strlen(default_config_path) + 1);
        strcpy(info->default_config_path, default_config_path);
    }

    fsm_info = dc_fsm_info_create(info->name, info->verbose_file);
    ret_val = dc_fsm_run(fsm_info, &from_state, &to_state, info, transitions);
    dc_fsm_info_destroy(&fsm_info);
    dc_application_lifecycle_destroy(&info->lifecycle);

    return ret_val;
}

static int create_settings(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->create_settings)
    {
        info->settings = info->lifecycle->create_settings();

        if(info->settings == NULL)
        {
            ret_val = -1;
        }
    }
    else
    {
        info->settings = NULL;
    }

    if(ret_val >= 0)
    {
        ret_val = PARSE_COMMAND_LINE;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int parse_command_line(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->parse_command_line)
    {
        ret_val = info->lifecycle->parse_command_line(info->settings, info->argc, info->argv);
    }

    if(ret_val >= 0)
    {
        ret_val = READ_ENV_VARS;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int read_env_vars(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->read_env_vars)
    {
        ret_val = info->lifecycle->read_env_vars(info->settings, environ);
    }

    if(ret_val >= 0)
    {
        ret_val = READ_CONFIG;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int read_config(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->read_config && info->default_config_path)
    {
        // this will not overwrite if it was set earlier, no need for an "if" to check if it was set already
        dc_setting_path_set(info->settings->config_path, info->default_config_path, DC_SETTING_NONE);
        ret_val = info->lifecycle->read_config(info->settings);
    }

    if(ret_val >= 0)
    {
        ret_val = SET_DEFAULTS;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int set_defaults(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->set_defaults)
    {
        ret_val = info->lifecycle->set_defaults(info->settings);
    }

    if(ret_val >= 0)
    {
        ret_val = RUN;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int run(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = info->lifecycle->run(info->settings);

    if(ret_val >= 0)
    {
        ret_val = CLEANUP;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int cleanup(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->cleanup)
    {
        ret_val = info->lifecycle->cleanup(info->settings);
    }

    if(ret_val >= 0)
    {
        ret_val = DESTROY_SETTINGS;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}


static int destroy_settings(void *arg)
{
    struct dc_application_info *info;
    int ret_val;

    info    = arg;
    ret_val = 0;

    if(info->lifecycle->destroy_settings)
    {
        dc_setting_path_destroy(&info->settings->config_path);
        ret_val = info->lifecycle->destroy_settings(&info->settings);
    }

    if(ret_val >= 0)
    {
        ret_val = DC_FSM_EXIT;
    }
    else
    {
        ret_val = ERROR;
    }

    return ret_val;
}

static int error(__attribute__((unused)) void *arg)
{
    return CLEANUP;
}
