#ifndef LIBDC_APPLICATION_OPTIONS_H
#define LIBDC_APPLICATION_OPTIONS_H

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
#include <dc_env/env.h>
#include <libconfig.h>


typedef void (*dc_setting_set_func)(const struct dc_env *env,
                                    struct dc_error *err,
                                    struct dc_setting *setting,
                                    const void *value, dc_setting_type type);

struct options
{
    struct dc_setting *setting;
    dc_setting_set_func setting_func;
    const char *name;
    int required;
    int val;
    const char *env_key;

    const void *(*read_from_string)(const struct dc_env *env,
                                    struct dc_error *err, const char *str);

    const char *config_key;

    const void *(*read_from_config)(const struct dc_env *env,
                                    struct dc_error *err, config_setting_t *item);

    const void *default_value;
};

struct dc_opt_settings
{
    struct dc_application_settings parent;
    struct options *opts;
    size_t opts_count;
    size_t opts_size;
    const char *flags;
    const char *env_prefix;
    int optind;
    int argc;
    char **argv;
};

void dc_options_set_string(const struct dc_env *env, struct dc_error *err,
                           struct dc_setting *setting, const void *value,
                           dc_setting_type type);

void dc_options_set_regex(const struct dc_env *env, struct dc_error *err,
                          struct dc_setting *setting, const void *value,
                          dc_setting_type type);

void dc_options_set_path(const struct dc_env *env, struct dc_error *err,
                         struct dc_setting *setting, const void *value,
                         dc_setting_type type);

void dc_options_set_bool(const struct dc_env *env, struct dc_error *err,
                         struct dc_setting *setting, const void *value,
                         dc_setting_type type);

void dc_options_set_uint16(const struct dc_env *env, struct dc_error *err,
                           struct dc_setting *setting, const void *value,
                           dc_setting_type type);

const void *dc_string_from_string(const struct dc_env *env,
                                  struct dc_error *err, const char *str);

const void *dc_flag_from_string(const struct dc_env *env,
                                struct dc_error *err, const char *str);

const void *dc_uint16_from_string(const struct dc_env *env,
                                  struct dc_error *err, const char *str);

#endif // LIBDC_APPLICATION_OPTIONS_H
