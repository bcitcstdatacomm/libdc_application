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

#include "command_line.h"
#include "options.h"
#include <dc_posix/dc_stdlib.h>
#include <getopt.h>

int dc_default_parse_command_line(const struct dc_posix_env *env,
                                  struct dc_error *err,
                                  struct dc_application_settings *settings,
                                  int argc,
                                  char *argv[])
{
    DC_TRACE(env);

    if(settings)
    {
        struct dc_opt_settings *opt_settings;
        size_t count;
        struct option *long_options;

        opt_settings = (struct dc_opt_settings *)settings;
        count = 0;

        while(opt_settings->opts[count].setting != NULL)
        {
            count++;
        }

        long_options = dc_calloc(env, err, count + 1, sizeof(struct option));

        if(dc_error_has_no_error(err))
        {
            for(size_t i = 0; i < count; i++)
            {
                long_options[i].name = opt_settings->opts[i].name;
                long_options[i].has_arg = opt_settings->opts[i].required;
                long_options[i].flag = NULL;
                long_options[i].val = opt_settings->opts[i].val;
            }

            while(1)
            {
                int c;
                const void *value;
                struct options *opt;

                c = getopt_long(argc, (char **)argv, opt_settings->flags, long_options, NULL);

                if(c == -1)
                {
                    break;
                }

                for(size_t i = 0; i < count; i++)
                {
                    if(opt_settings->opts[i].val == c)
                    {
                        opt = &opt_settings->opts[i];
                        break;
                    }
                }

                value = opt->read_from_string(env, err, optarg);

                if(dc_error_has_no_error(err))
                {
                    opt->setting_func(env, err, opt->setting, value, DC_SETTING_COMMAND_LINE);
                }

                if(dc_error_has_error(err))
                {
                    // TODO: now what?
                }
            }

            opt_settings->optind = optind;
            opt_settings->argc = argc;
            opt_settings->argv = argv;

            dc_free(env, long_options, (count + 1) * sizeof(struct option));
        }
    }

    return 0;
}
