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
#include <dc_util/path.h>
#include <dc_posix/stdlib.h>
#include <dc_posix/string.h>
#include <regex.h>
#include <stdio.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
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
#pragma GCC diagnostic pop

bool dc_setting_is_set(const struct dc_posix_env *env, struct dc_setting *setting)
{
    DC_TRACE(env);

    return setting->type != DC_SETTING_NONE;
}

struct dc_setting_path *dc_setting_path_create(const struct dc_posix_env *env, struct dc_error *err)
{
    struct dc_setting_path *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_path));

    if(DC_HAS_NO_ERROR(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->path        = NULL;
    }

    return setting;
}

void dc_setting_path_destroy(const struct dc_posix_env *env, struct dc_setting_path **psetting)
{
    struct dc_setting_path *setting;

    DC_TRACE(env);
    setting = *psetting;

    if(setting->path)
    {
        size_t length;

        length = dc_strlen(env, setting->path);
        dc_free(env, setting->path, length);
    }

    dc_free(env, *psetting, sizeof(struct dc_setting_path));

    if(env->null_free)
    {
        *psetting = NULL;
    }
}

bool dc_setting_path_set(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting_path *setting, const char *value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        expand_path(env, err, &setting->path, value);

        if(DC_HAS_NO_ERROR(err))
        {
            setting->parent.type = type;
            ret_val = true;
        }
    }

    return ret_val;
}

const char *dc_setting_path_get(const struct dc_posix_env *env, struct dc_setting_path *setting)
{
    DC_TRACE(env);

    return setting->path;
}

struct dc_setting_string *dc_setting_string_create(const struct dc_posix_env *env, struct dc_error *err)
{
    struct dc_setting_string *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_string));

    if(DC_HAS_NO_ERROR(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->string      = NULL;
    }

    return setting;
}

void dc_setting_string_destroy(const struct dc_posix_env *env, struct dc_setting_string **psetting)
{
    struct dc_setting_string *setting;
    size_t length;

    DC_TRACE(env);
    setting = *psetting;
    length  = dc_strlen(env, setting->string);
    dc_free(env, setting->string, length);
    dc_free(env, *psetting, sizeof(struct dc_setting_string));

    if(env->null_free)
    {
        *psetting = NULL;
    }
}

bool dc_setting_string_set(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting_string *setting, const char *value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->string = dc_malloc(env, err, (dc_strlen(env, value) + 1) * sizeof(char));

        if(DC_HAS_NO_ERROR(err))
        {
            dc_strcpy(env, setting->string, value);
            setting->parent.type = type;
            ret_val              = true;
        }
    }

    return ret_val;
}

const char *dc_setting_string_get(const struct dc_posix_env *env, struct dc_setting_string *setting)
{
    DC_TRACE(env);

    return setting->string;
}

struct dc_setting_regex *dc_setting_regex_create(const struct dc_posix_env *env, struct dc_error *err, const char *pattern)
{
    struct dc_setting_regex *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_regex));

    if(DC_HAS_NO_ERROR(err))
    {
        int result = regcomp(&setting->regex, pattern, REG_EXTENDED);

        if(result == 0)
        {
            setting->parent.type = DC_SETTING_NONE;
            setting->pattern     = pattern;
            setting->string      = NULL;
        }
        else
        {
            DC_REPORT_USER(env, err, "XXX", result);
        }
    }

    return setting;
}

void dc_setting_regex_destroy(const struct dc_posix_env *env, struct dc_setting_regex **psetting)
{
    struct dc_setting_regex *setting;

    DC_TRACE(env);
    setting = *psetting;
    regfree(&setting->regex);
    dc_free(env, setting, sizeof(struct dc_setting_path));

    if(env->null_free)
    {
        *psetting = NULL;
    }
}

bool dc_setting_regex_set(const struct dc_posix_env *env, struct dc_error *err, struct dc_setting_regex *setting, const char *value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);
    ret_val = false;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        int match;

        match = regexec(&setting->regex, value, 0, NULL, 0);

        if(match == 0)
        {
            setting->string = dc_malloc(env, err, (dc_strlen(env, value) + 1) * sizeof(char));

            if(DC_HAS_NO_ERROR(err))
            {
                dc_strcpy(env, setting->string, value);
                setting->parent.type = type;
                ret_val = true;
            }
        }
        else
        {
            static const char   *msg_format = "Bad string: %s, must match %s";
            static const size_t  msg_format_length = 25;
            size_t               msg_length;
            struct dc_error      local_err;
            char                *msg;

            msg_length = msg_format_length + dc_strlen(env, setting->pattern) + dc_strlen(env, value) + 1;
            dc_error_init(&local_err);
            msg = dc_malloc(env, &local_err, msg_length * sizeof(char));

            if(DC_HAS_NO_ERROR(&local_err))
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
                sprintf(msg, msg_format, value, setting->pattern);
#pragma GCC diagnostic pop
                DC_REPORT_USER(env, err, msg, match);
                dc_free(env, msg, msg_length * sizeof(char));
            }
            else
            {
                DC_REPORT_SYSTEM(env, err, "Out of memory", ENOMEM);
            }
        }
    }

    return ret_val;
}

const char *dc_setting_regex_get(const struct dc_posix_env *env, struct dc_setting_regex *setting)
{
    DC_TRACE(env);

    return setting->string;
}

struct dc_setting_bool *dc_setting_bool_create(const struct dc_posix_env *env, struct dc_error *err)
{
    struct dc_setting_bool *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_bool));

    if(DC_HAS_NO_ERROR(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->value       = false;
    }

    return setting;
}

void dc_setting_bool_destroy(const struct dc_posix_env *env, struct dc_setting_bool **psetting)
{
    DC_TRACE(env);
    dc_free(env, *psetting, sizeof(struct dc_setting_bool));

    if(env->null_free)
    {
        *psetting = NULL;
    }
}

bool dc_setting_bool_set(const struct dc_posix_env *env, struct dc_setting_bool *setting, bool value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value       = value;
        ret_val              = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

bool dc_setting_bool_get(const struct dc_posix_env *env, struct dc_setting_bool *setting)
{
    DC_TRACE(env);

    return setting->value;
}

struct dc_setting_uint16 *dc_setting_uint16_create(const struct dc_posix_env *env, struct dc_error *err)
{
    struct dc_setting_uint16 *setting;

    DC_TRACE(env);
    setting = dc_malloc(env, err, sizeof(struct dc_setting_uint16));

    if(DC_HAS_NO_ERROR(err))
    {
        setting->parent.type = DC_SETTING_NONE;
        setting->value       = 0;
    }

    return setting;
}

void dc_setting_uint16_destroy(const struct dc_posix_env *env, struct dc_setting_uint16 **psetting)
{
    DC_TRACE(env);
    dc_free(env, *psetting, sizeof(struct dc_setting_uint16));

    if(env->null_free)
    {
        *psetting = NULL;
    }
}

bool dc_setting_uint16_set(const struct dc_posix_env *env, struct dc_setting_uint16 *setting, uint16_t value, dc_setting_type type)
{
    bool ret_val;

    DC_TRACE(env);

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value       = value;
        ret_val              = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

uint16_t dc_setting_uint16_get(const struct dc_posix_env *env, struct dc_setting_uint16 *setting)
{
    DC_TRACE(env);

    return setting->value;
}
