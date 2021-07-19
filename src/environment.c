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


#include "environment.h"
#include "options.h"
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>


static bool set_from_env(const struct dc_posix_env *env,
                         struct dc_error *err,
                         struct dc_opt_settings *settings,
                         size_t prefix_len,
                         const char *key,
                         const char *value);


int dc_default_read_env_vars(const struct dc_posix_env *env, struct dc_error *err, struct dc_application_settings *settings, char **envvars)
{
    struct dc_opt_settings *opt_settings;
    const char             *prefix;
    size_t                  prefix_len;

    DC_TRACE(env);
    opt_settings = (struct dc_opt_settings *)settings;
    prefix       = opt_settings->env_prefix;
    prefix_len   = dc_strlen(env, prefix);

    while(*envvars)
    {
        if(dc_strncmp(env, *envvars, prefix, prefix_len) == 0)
        {
            size_t  length;
            char   *env_var;

            length  = dc_strlen(env, *envvars) + 1;
            env_var = dc_malloc(env, err, length * sizeof(char));

            if(dc_error_has_no_error(err))
            {
                char *rest;
                char *key;
                char *value;

                dc_strcpy(env, env_var, *envvars);
                rest  = NULL;

                // TODO: what if no token found??
                key   = dc_strtok_r(env, env_var, "=", &rest);

                // TODO: why am I using ?
                // TODO: what if no token found?
                value = dc_strtok_r(env, NULL, "=", &rest);

                // TODO: what to do about an err?
                set_from_env(env, err, opt_settings, prefix_len, key, value);
                dc_free(env, env_var, length * sizeof(char));
            }
        }

        envvars++;
    }

    return 0;
}

static bool set_from_env(const struct dc_posix_env *env,
                         struct dc_error           *err,
                         struct dc_opt_settings    *settings,
                         size_t                     prefix_len,
                         const char                *env_key,
                         const char                *env_value)
{
    const char *sub_key;
    bool        found;

    DC_TRACE(env);
    sub_key = &env_key[prefix_len];
    found = false;

    for(size_t i = 0; settings->opts[i].name != NULL; i++)
    {
        struct options *opt;

        opt = &settings->opts[i];

        if(dc_strcmp(env, sub_key, opt->env_key) == 0)
        {
            const void *value;

            value = opt->read_from_string(env, err, env_value);
            opt->setting_func(env, err, opt->setting, value, DC_SETTING_ENVIRONMENT);

            // TODO: what to do about an err?
            found = true;
        }
    }

    return found;
}
