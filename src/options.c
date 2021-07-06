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


#include "options.h"
#include <dc_posix/stdlib.h>
#include <dc_util/types.h>


void dc_options_set_string(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    dc_setting_string_set(env, err, (struct dc_setting_string *)setting, (const char *)value, type);
}


void dc_options_set_regex(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    dc_setting_regex_set(env, err, (struct dc_setting_regex *)setting, (const char *)value, type);
}


void dc_options_set_path(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    dc_setting_path_set(env, err, (struct dc_setting_path *)setting, (const char *)value, type);
}


void dc_options_set_bool(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const bool *pbool;

    pbool = value;
    dc_setting_bool_set(env, (struct dc_setting_bool *)setting, *pbool, type);
}

void dc_options_set_uint16(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const uint16_t *punit16;

    punit16 = value;
    dc_setting_uint16_set(env, (struct dc_setting_uint16 *)setting, *punit16, type);
}

const void *dc_string_from_string(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err, const char *str)
{
    DC_TRACE(env);

    return str;
}

const void *dc_flag_from_string(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err, __attribute__((unused))  const char *str)
{
    static bool value = true;

    DC_TRACE(env);

    return &value;
}

const void *dc_uint16_from_string(const struct dc_posix_env *env, struct dc_error *err, const char *str)
{
    uint16_t *value;

    DC_TRACE(env);
    value = dc_malloc(env, err, sizeof(uint16_t));

    if(DC_HAS_NO_ERROR(err))
    {
        *value = dc_uint16_from_str(env, err, str, 10);

        if(DC_HAS_ERROR(err))
        {
            dc_free(env, value, sizeof(uint16_t));
            value = NULL;
        }
    }

    return value;
}
