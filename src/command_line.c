#include "command_line.h"
#include "options.h"
#include <getopt.h>
#include <stdlib.h>


int dc_default_parse_command_line(struct dc_application_settings *settings, int argc, char *argv[])
{
    struct dc_opt_settings *opt_settings;
    size_t                       count;
    struct option               *long_options;

    opt_settings = (struct dc_opt_settings *)settings;
    count        = 0;

    while(opt_settings->opts[count].setting != NULL)
    {
        count++;
    }

    long_options = calloc(count + 1, sizeof(struct option));

    for(size_t i = 0; i < count; i++)
    {
        long_options[i].name    = opt_settings->opts[i].name;
        long_options[i].has_arg = opt_settings->opts[i].required;
        long_options[i].flag    = 0;
        long_options[i].val     = opt_settings->opts[i].val;
    }

    while(1)
    {
        int             c;
        int             option_index;
        const void     *value;
        struct options *opt;

        option_index = 0;
        c = getopt_long(argc, (char **)argv, opt_settings->flags,
                        long_options, &option_index);

        if(c == -1)
        {
            break;
        }

        opt   = &opt_settings->opts[option_index];
        value = opt->read_from_string(settings->env, optarg);
        opt->setting_func(settings->env, opt->setting, value, DC_SETTING_COMMAND_LINE);
    }

    return 0;
}



