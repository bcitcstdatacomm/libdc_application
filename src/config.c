#include "config.h"
#include "options.h"
#include <dc_posix/stdlib.h>


int dc_default_load_config(struct dc_application_settings *settings)
{
    const char *config_path;
    config_t    config;

    config_path  = dc_setting_path_get(settings->config_path);

    config_init(&config);

    if(!(config_read_file(&config, config_path)))
    {
        // if the config file was passed in on the command line or set as an env var then it needs to exist
        if(dc_setting_is_set((struct dc_setting *)settings->config_path))
        {
            fprintf(stderr, "%s:%d - %s\n", config_error_file(&config),
                    config_error_line(&config), config_error_text(&config));
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

                item  = config_lookup(&config, opt->config_key);

                if(item != NULL)
                {
                    value = opt->read_from_config(settings->env, item);
                    opt->setting_func(settings->env, opt->setting, value, DC_SETTING_CONFIG);
                }
            }
        }
    }

    config_destroy(&config);

    return 0;
}


const void *dc_string_from_config(__attribute__((unused)) const struct dc_posix_env *env, config_setting_t *item)
{
    return item->value.sval;
}

const void *dc_flag_from_config(__attribute__((unused)) const struct dc_posix_env *env, config_setting_t *item)
{
    static bool true_value = true;
    static bool false_value = false;
    int value;

    value = item->value.ival;

    if(value)
    {
        return &true_value;
    }

    return &false_value;
}

const void *dc_uint16_from_config(const struct dc_posix_env *env, config_setting_t *item)
{
    int       err;
    long long config_value;
    uint16_t *value;

    config_value = item->value.llval;

    if(config_value < 0 || config_value > UINT16_MAX)
    {
        return NULL;
    }

    value = dc_malloc(env, &err, sizeof(uint16_t));

    if(value == NULL)
    {
    }

    *value = (uint16_t)config_value;

    return value;
}