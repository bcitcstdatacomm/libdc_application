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


void dc_options_set_path(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    dc_setting_path_set(env, (struct dc_setting_path *)setting, (const char *)value, type);
}

void dc_options_set_bool(__attribute__((unused)) const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const bool *pbool;

    pbool = value;
    dc_setting_bool_set(env, (struct dc_setting_bool *)setting, *pbool, type);
}

void dc_options_set_uint16(__attribute__((unused)) const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const uint16_t *punit16;

    punit16 = value;
    dc_setting_uint16_set(env, (struct dc_setting_uint16 *)setting, *punit16, type);
}

const void *dc_string_from_string(const struct dc_posix_env *env, const char *str)
{
    DC_TRACE(env);

    return str;
}

const void *dc_flag_from_string(const struct dc_posix_env *env, __attribute__((unused))  const char *str)
{
    static bool value = true;

    DC_TRACE(env);

    return &value;
}

const void *dc_uint16_from_string(const struct dc_posix_env *env, const char *str)
{
    int       err;
    uint16_t *value;

    DC_TRACE(env);
    value = dc_malloc(env, &err, sizeof(uint16_t));

    if(value == NULL)
    {
    }

    *value = dc_uint16_from_str(env, &err, str, 10);

    return value;
}
