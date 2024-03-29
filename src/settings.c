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


#include "dc_application/settings.h"
#include <dc_c/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <dc_posix/dc_regex.h>
#include <dc_util/path.h>


struct dc_setting_string
{
    struct dc_setting parent;
    char *string;
};

struct dc_setting_regex
{
    struct dc_setting parent;
    const char *pattern;
    regex_t regex;
    char *string;
};

struct dc_setting_path
{
    struct dc_setting parent;
    char *path;
};

struct dc_setting_bool
{
    struct dc_setting parent;
    bool value;
};

struct dc_setting_uint16
{
    struct dc_setting parent;
    uint16_t value;
};

struct dc_setting_in_port_t
{
    struct dc_setting parent;
    in_port_t value;
};

bool dc_setting_is_set(const struct dc_env *env, struct dc_setting *setting)
{
    DC_TRACE(env);

    return setting->type != DC_SETTING_NONE;
}

struct dc_setting_path *dc_setting_path_create(const struct dc_env *env, struct dc_error *err)
{
    struct dc_setting_path *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_path));

    if(dc_error_has_no_error(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->path = NULL;
    }

    return setting;
}

void dc_setting_path_destroy(const struct dc_env *env, struct dc_setting_path **psetting)
{
    struct dc_setting_path *setting;

    DC_TRACE(env);
    setting = *psetting;

    if(setting->path)
    {
        dc_free(env, setting->path);
    }

    dc_free(env, *psetting);
    *psetting = NULL;
}

bool dc_setting_path_set(const struct dc_env *env,
                         struct dc_error *err,
                         struct dc_setting_path *setting,
                         const char *value,
                         dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        if(value)
        {
            dc_expand_path(env, err, &setting->path, value);

            if(dc_error_has_no_error(err))
            {
                setting->parent.type = type;
                ret_val = true;
            }
        }
    }

    return ret_val;
}

const char *dc_setting_path_get(const struct dc_env *env, struct dc_setting_path *setting)
{
    DC_TRACE(env);

    return setting->path;
}

struct dc_setting_string *dc_setting_string_create(const struct dc_env *env, struct dc_error *err)
{
    struct dc_setting_string *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_string));

    if(dc_error_has_no_error(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->string = NULL;
    }

    return setting;
}

void dc_setting_string_destroy(const struct dc_env *env, struct dc_setting_string **psetting)
{
    struct dc_setting_string *setting;

    DC_TRACE(env);
    setting = *psetting;

    if(setting->string)
    {
        dc_free(env, setting->string);
    }

    dc_free(env, *psetting);
    *psetting = NULL;
}

bool dc_setting_string_set(const struct dc_env *env,
                           struct dc_error *err,
                           struct dc_setting_string *setting,
                           const char *value,
                           dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        size_t len;

        len = dc_strlen(env, value);
        setting->string = dc_malloc(env, err, (len + 1) * sizeof(char));

        if(dc_error_has_no_error(err))
        {
            dc_strcpy(env, setting->string, value);
            setting->parent.type = type;
            ret_val = true;
        }
    }

    return ret_val;
}

const char *dc_setting_string_get(const struct dc_env *env, struct dc_setting_string *setting)
{
    DC_TRACE(env);

    return setting->string;
}

struct dc_setting_regex *
dc_setting_regex_create(const struct dc_env *env, struct dc_error *err, const char *pattern)
{
    struct dc_setting_regex *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_regex));

    if(dc_error_has_no_error(err))
    {
        dc_regcomp(env, err, &setting->regex, pattern, REG_EXTENDED);

        if(dc_error_has_no_error(err))
        {
            setting->parent.type = DC_SETTING_NONE;
            setting->pattern = pattern;
            setting->string = NULL;
        }
    }

    return setting;
}

void dc_setting_regex_destroy(const struct dc_env *env, struct dc_setting_regex **psetting)
{
    struct dc_setting_regex *setting;

    DC_TRACE(env);
    setting = *psetting;

    if(setting->string)
    {
        dc_free(env, setting->string);
    }

    dc_regfree(env, &setting->regex);
    dc_free(env, setting);
    *psetting = NULL;
}

bool dc_setting_regex_set(const struct dc_env *env,
                          struct dc_error *err,
                          struct dc_setting_regex *setting,
                          const char *value,
                          dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        int match;

        match = dc_regexec(env, &setting->regex, value, 0, NULL, 0);

        if(match == 0)
        {
            size_t len;

            len = dc_strlen(env, value);
            setting->string = dc_malloc(env, err, (len + 1) * sizeof(char));

            if(dc_error_has_no_error(err))
            {
                dc_strcpy(env, setting->string, value);
                setting->parent.type = type;
                ret_val = true;
            }
        }
        else
        {
            // TODO: what do you do if the value doesn't match the regex
        }
    }

    return ret_val;
}

const char *dc_setting_regex_get(const struct dc_env *env, struct dc_setting_regex *setting)
{
    DC_TRACE(env);

    return setting->string;
}

struct dc_setting_bool *dc_setting_bool_create(const struct dc_env *env, struct dc_error *err)
{
    struct dc_setting_bool *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_bool));

    if(dc_error_has_no_error(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->value = false;
    }

    return setting;
}

void dc_setting_bool_destroy(const struct dc_env *env, struct dc_setting_bool **psetting)
{
    DC_TRACE(env);
    dc_free(env, *psetting);
    *psetting = NULL;
}

bool dc_setting_bool_set(const struct dc_env *env,
                         struct dc_setting_bool *setting,
                         bool value,
                         dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value = value;
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

bool dc_setting_bool_get(const struct dc_env *env, struct dc_setting_bool *setting)
{
    DC_TRACE(env);

    return setting->value;
}

struct dc_setting_uint16 *dc_setting_uint16_create(const struct dc_env *env, struct dc_error *err)
{
    struct dc_setting_uint16 *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_uint16));

    if(dc_error_has_no_error(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->value = 0;
    }

    return setting;
}

void dc_setting_uint16_destroy(const struct dc_env *env, struct dc_setting_uint16 **psetting)
{
    DC_TRACE(env);
    dc_free(env, *psetting);
    *psetting = NULL;
}

bool dc_setting_uint16_set(const struct dc_env *env,
                           struct dc_setting_uint16 *setting,
                           uint16_t value,
                           dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value = value;
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

uint16_t dc_setting_uint16_get(const struct dc_env *env, struct dc_setting_uint16 *setting)
{
    DC_TRACE(env);

    return setting->value;
}


struct dc_setting_in_port_t *dc_setting_in_port_t_create(const struct dc_env *env, struct dc_error *err)
{
    struct dc_setting_in_port_t *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_uint16));

    if(dc_error_has_no_error(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->value = 0;
    }

    return setting;
}

void dc_setting_in_port_t_destroy(const struct dc_env *env,  struct dc_setting_in_port_t **psetting)
{
    DC_TRACE(env);
    dc_free(env, *psetting);
    *psetting = NULL;
}

bool dc_setting_in_port_t_set(const struct dc_env *env, struct dc_setting_in_port_t *setting, in_port_t value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value = value;
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

in_port_t dc_setting_in_port_t_get(const struct dc_env *env, struct dc_setting_in_port_t *setting)
{
    DC_TRACE(env);

    return setting->value;
}

