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
#include <string.h>
#include <dc_posix/stdlib.h>
#include <stdlib.h>


static bool set_from_env(const struct dc_posix_env *env,
                         struct dc_opt_settings *settings,
                         size_t prefix_len,
                         const char *key,
                         const char *value);


int dc_default_read_env_vars(const struct dc_posix_env *env, struct dc_application_settings *settings, char **envvars)
{
    struct dc_opt_settings *opt_settings;
    const char             *prefix;
    size_t                  prefix_len;

    DC_TRACE(env);
    opt_settings = (struct dc_opt_settings *)settings;
    prefix       = opt_settings->env_prefix;
    prefix_len   = strlen(prefix);

    while(*envvars)
    {
        if(strncmp(*envvars, prefix, prefix_len) == 0)
        {
            char   *env_var;
            size_t  length;
            char   *rest;
            char   *key;
            char   *value;
            int     err;

            length  = strlen(*envvars) + 1;
            env_var = dc_malloc(env, &err, length * sizeof(char));

            if(env_var == NULL)
            {
            }

            strcpy(env_var, *envvars);
            rest  = NULL;
            key   = strtok_r(env_var, "=", &rest);
            value = strtok_r(NULL, "=", &rest);
            set_from_env(env, opt_settings, prefix_len, key, value);
            free(env_var);
        }

        envvars++;
    }

    return 0;
}

static bool set_from_env(const struct dc_posix_env *env,
                         struct dc_opt_settings *settings,
                         size_t prefix_len,
                         const char *env_key,
                         const char *env_value)
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

        if(strcmp(sub_key, opt->env_key) == 0)
        {
            const void *value;

            value = opt->read_from_string(env, env_value);
            opt->setting_func(env, opt->setting, value, DC_SETTING_ENVIRONMENT);
            found = true;
        }
    }

    return found;
}
