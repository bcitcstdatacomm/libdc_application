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

#include "defaults.h"
#include "options.h"

int dc_default_set_defaults(const struct dc_posix_env *env,
                            struct dc_error *err,
                            struct dc_application_settings *settings)
{
    struct dc_opt_settings *opt_settings;

    DC_TRACE(env);
    opt_settings = (struct dc_opt_settings *)settings;

    for(size_t i = 0; opt_settings->opts[i].name != NULL; i++)
    {
        struct options *opt;

        opt = &opt_settings->opts[i];

        if(opt->default_value)
        {
            opt->setting_func(env, err, opt->setting, opt->default_value, DC_SETTING_DEFAULT);

            if(dc_error_has_error(err))
            {
                // TODO: now what?
            }
        }
    }

    return 0;
}
