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
#include <stdio.h>


struct dc_application_info;
struct dc_application_lifecycle;

struct dc_application_settings
{
    struct dc_setting_path *config_path;
};

struct dc_application_lifecycle *dc_application_lifecycle_create(struct dc_application_settings *(*create_settings_func)(void), int (*destroy_settings_func)(struct dc_application_settings **), int (*run_func)(struct dc_application_settings *));
void dc_application_lifecycle_destroy(struct dc_application_lifecycle **plifecycle);
void dc_application_lifecycle_set_set_defaults(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *settings));
void dc_application_lifecycle_set_read_config(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *));
void dc_application_lifecycle_set_read_env_vars(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *settings, char **env));
void dc_application_lifecycle_set_parse_command_line(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *settings, int argc, char *argv[]));
void dc_application_lifecycle_set_cleanup(struct dc_application_lifecycle *lifecycle, int (*func)(struct dc_application_settings *settings));
struct dc_application_info *dc_application_info_create(const char *name, FILE *verbose_file);
void dc_application_info_destroy(struct dc_application_info **pinfo);
int dc_application_run(struct dc_application_info *info,
                       struct dc_application_lifecycle *(*create_lifecycle_func)(void),
                       const char *default_config_path,
                       int argc,
                       char *argv[]);

#endif // LIBDC_APPLICATION_APPLICATION_H
