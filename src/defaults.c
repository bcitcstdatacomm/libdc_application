#include "defaults.h"
#include "options.h"


int dc_default_set_defaults(struct dc_application_settings *settings)
{
    struct dc_opt_settings *opt_settings;

    opt_settings = (struct dc_opt_settings *)settings;

    for(size_t i = 0; opt_settings->opts[i].name != NULL; i++)
    {
        struct options *opt;

        opt = &opt_settings->opts[i];

        if(opt->default_value)
        {
            opt->setting_func(settings->env, opt->setting, opt->default_value, DC_SETTING_DEFAULT);
        }
    }

    return 0;
}
