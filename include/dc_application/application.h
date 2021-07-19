#ifndef LIBDC_APPLICATION_APPLICATION_H
#define LIBDC_APPLICATION_APPLICATION_H


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


#include "settings.h"
#include <dc_posix/dc_posix_env.h>
#include <stdio.h>


struct dc_application_info;
struct dc_application_lifecycle;

struct dc_application_settings
{
    struct dc_setting_path *config_path;
};

/**
 *
 * @param create_settings_func
 * @param destroy_settings_func
 * @param run_func
 * @return
 */
struct dc_application_lifecycle *dc_application_lifecycle_create(const struct dc_posix_env *env,
                                                                 struct dc_error *err,
                                                                 struct dc_application_settings *(*create_settings_func)(const struct dc_posix_env *env, struct dc_error *err),
                                                                 int (*destroy_settings_func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings **),
                                                                 int (*run_func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *));
/**
 *
 * @param plifecycle
 */
void dc_application_lifecycle_destroy(const struct dc_posix_env *env, struct dc_application_lifecycle **plifecycle);

/**
 *
 * @param lifecycle
 * @param func
 */
void dc_application_lifecycle_set_set_defaults(const struct dc_posix_env *env, struct dc_application_lifecycle *lifecycle, int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings));

/**
 *
 * @param lifecycle
 * @param func
 */
void dc_application_lifecycle_set_read_config(const struct dc_posix_env *env, struct dc_application_lifecycle *lifecycle, int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *));

/**
 *
 * @param lifecycle
 * @param func
 */
void dc_application_lifecycle_set_read_env_vars(const struct dc_posix_env *env, struct dc_application_lifecycle *lifecycle, int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings, char **envvars));

/**
 *
 * @param lifecycle
 * @param func
 */
void dc_application_lifecycle_set_parse_command_line(const struct dc_posix_env *env, struct dc_application_lifecycle *lifecycle, int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings, int argc, char *argv[]));

/**
 *
 * @param lifecycle
 * @param func
 */
void dc_application_lifecycle_set_cleanup(const struct dc_posix_env *env, struct dc_application_lifecycle *lifecycle, int (*func)(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings));


/**
 *
 * @param name
 * @param verbose_file
 * @return
 */
struct dc_application_info *dc_application_info_create(const struct dc_posix_env *env,
                                                       struct dc_error           *err,
                                                       const char                *name,
                                                       FILE                      *verbose_file);

/**
 *
 * @param pinfo
 */
void dc_application_info_destroy(const struct dc_posix_env *env, struct dc_application_info **pinfo);


/**
 *
 * @param env
 * @param err
 * @param info
 * @param create_lifecycle_func
 * @param destroy_lifecycle_func
 * @param default_config_path
 * @param argc
 * @param argv
 * @return
 */
int dc_application_run(const struct dc_posix_env       *env,
                       struct dc_error                 *err,
                       struct dc_application_info      *info,
                       struct dc_application_lifecycle *(*create_lifecycle_func)(const struct dc_posix_env *env, struct dc_error *err),
                       void (*destroy_lifecycle_func)(const struct dc_posix_env *env, struct dc_application_lifecycle **plifecycle),
                       const char                      *default_config_path,
                       int                              argc,
                       char                            *argv[]);


#endif // LIBDC_APPLICATION_APPLICATION_H
