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

#include "config.h"
#include "options.h"
#include <dc_c/dc_stdlib.h>

int dc_default_load_config(const struct dc_env *env,
                           struct dc_error *err,
                           struct dc_application_settings *settings)
{
    const char *config_path;
    config_t config;

    DC_TRACE(env);
    config_path = dc_setting_path_get(env, settings->config_path);

    config_init(&config);

    if(!(config_read_file(&config, config_path)))
    {
        // if the config file was passed in on the command line or set as an env var then it needs to exist
        if(dc_setting_is_set(env, (struct dc_setting *)settings->config_path))
        {
            // TODO: this should be an error somehow - time to figure that out!
            fprintf(stderr,                     // NOLINT(cert-err33-c)
                    "%s:%d - %s\n",
                    config_error_file(&config),
                    config_error_line(&config),
                    config_error_text(&config));
            config_destroy(&config);

            return -1;
        }
    }
    else
    {
        struct dc_opt_settings *opt_settings;

        opt_settings = (struct dc_opt_settings *)settings;

        for(size_t i = 0; opt_settings->opts[i].name != NULL; i++)
        {
            struct options *opt;

            opt = &opt_settings->opts[i];

            if(opt->config_key)
            {
                config_setting_t *item;
                const void *value;

                item = config_lookup(&config, opt->config_key);

                if(item != NULL)
                {
                    value = opt->read_from_config(env, err, item);

                    if(dc_error_has_no_error(err))
                    {
                        opt->setting_func(env, err, opt->setting, value, DC_SETTING_CONFIG);
                    }

                    if(dc_error_has_error(err))
                    {
                        // TODO: now what?
                    }
                }
            }
        }
    }

    config_destroy(&config);

    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
const void *dc_string_from_config(const struct dc_env *env,
                                  struct dc_error *err,
                                  config_setting_t *item)
{
    DC_TRACE(env);

    return item->value.sval;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
const void *dc_flag_from_config(const struct dc_env *env,
                                struct dc_error *err,
                                config_setting_t *item)
{
    static bool true_value = true;
    static bool false_value = false;
    int value;

    DC_TRACE(env);
    value = item->value.ival;

    if(value)
    {
        return &true_value;
    }

    return &false_value;
}
#pragma GCC diagnostic pop

const void *dc_uint16_from_config(const struct dc_env *env, struct dc_error *err, config_setting_t *item)
{
    long long config_value;
    uint16_t *value;

    DC_TRACE(env);
    config_value = item->value.llval;

    if(config_value < 0 || config_value > UINT16_MAX)
    {
        return NULL;
    }

    value = dc_malloc(env, err, sizeof(uint16_t));

    if(dc_error_has_no_error(err))
    {
        *value = (uint16_t)config_value;
    }

    return value;
}
