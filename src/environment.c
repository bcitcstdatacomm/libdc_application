#include "environment.h"
#include "options.h"
#include <string.h>
#include <dc_posix/stdlib.h>
#include <stdlib.h>


static bool set_from_env(struct dc_opt_settings *settings,
                         size_t prefix_len,
                         const char *key,
                         const char *value);


int dc_default_read_env_vars(struct dc_application_settings *settings, char **env)
{
    struct dc_opt_settings *opt_settings;
    const char             *prefix;
    size_t                  prefix_len;

    opt_settings = (struct dc_opt_settings *)settings;
    prefix       = opt_settings->env_prefix;
    prefix_len   = strlen(prefix);

    while(*env)
    {
        if(strncmp(*env, prefix, prefix_len) == 0)
        {
            char   *env_var;
            size_t  length;
            char   *rest;
            char   *key;
            char   *value;
            int     err;

            length  = strlen(*env) + 1;
            env_var = dc_malloc(settings->env, &err, length * sizeof(char));

            if(env_var == NULL)
            {
            }

            strcpy(env_var, *env);
            rest  = NULL;
            key   = strtok_r(env_var, "=", &rest);
            value = strtok_r(NULL, "=", &rest);
            set_from_env(opt_settings, prefix_len, key, value);
            free(env_var);
        }

        env++;
    }

    return 0;
}

static bool set_from_env(struct dc_opt_settings *settings,
                         size_t prefix_len,
                         const char *env_key,
                         const char *env_value)
{
    const char *sub_key;
    bool        found;

    sub_key = &env_key[prefix_len];
    found = false;

    for(size_t i = 0; settings->opts[i].name != NULL; i++)
    {
        struct options *opt;

        opt = &settings->opts[i];

        if(strcmp(sub_key, opt->env_key) == 0)
        {
            const void *value;

            value = opt->read_from_string(settings->parent.env, env_value);
            opt->setting_func(settings->parent.env, opt->setting, value, DC_SETTING_ENVIRONMENT);
            found = true;
        }
    }

    return found;
}
