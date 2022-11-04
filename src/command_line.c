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
#include <dc_c/dc_stdlib.h>
#include <dc_unix/dc_getopt.h>


static struct option *create_long_opts(const struct dc_env *env,
                                       struct dc_error *err,
                                       const struct dc_opt_settings *opt_settings,
                                       size_t count);
static void parse_arguments(const struct dc_env *env,
                            struct dc_error *err,
                            int argc,
                            char *argv[],
                            const struct dc_opt_settings *opt_settings,
                            const struct option *long_options,
                            size_t count);


int dc_default_parse_command_line(const struct dc_env *env,
                                  struct dc_error *err,
                                  struct dc_application_settings *settings,
                                  int argc,
                                  char *argv[])
{
    DC_TRACE(env);

    if(settings)
    {
        struct dc_opt_settings *opt_settings;
        struct option *long_options;
        size_t count;

        opt_settings = (struct dc_opt_settings *)settings;
        count = 0;

        while(opt_settings->opts[count].setting != NULL)
        {
            count++;
        }

        long_options = create_long_opts(env, err, opt_settings, count);

        if(dc_error_has_no_error(err))
        {
            parse_arguments(env, err, argc, argv, opt_settings, long_options, count);
            opt_settings->optind = optind;
            opt_settings->argc = argc;
            opt_settings->argv = argv;

            dc_free(env, long_options);
        }
    }

    return 0;
}


static struct option *create_long_opts(const struct dc_env *env,
                                       struct dc_error *err,
                                       const struct dc_opt_settings *opt_settings,
                                       size_t count)
{
    struct option *long_options;

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
    }

    return long_options;
}


static void parse_arguments(const struct dc_env *env,
                            struct dc_error *err,
                            int argc,
                            char *argv[],
                            const struct dc_opt_settings *opt_settings,
                            const struct option *long_options,
                            size_t count)
{
    while(1)
    {
        int c;
        const void *value;
        struct options *opt;

        c = dc_getopt_long(env, argc, (char **)argv, opt_settings->flags, long_options, NULL);

        if(c == -1)
        {
            break;
        }

        opt = NULL;

        for(size_t i = 0; i < count; i++)
        {
            if(opt_settings->opts[i].val == c)
            {
                opt = &opt_settings->opts[i];
                break;
            }
        }

        if(opt == NULL)
        {
            // TODO: fix
            DC_ERROR_RAISE_USER(err, "", 100);
        }
        else
        {
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
    }
}
