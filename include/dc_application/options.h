#ifndef LIBDC_APPLICATION_OPTIONS_H
#define LIBDC_APPLICATION_OPTIONS_H


#include "application.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#include <libconfig.h>
#pragma GCC diagnostic pop


typedef void (*dc_setting_set_func)(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type);


struct options
{
    struct dc_setting *setting;
    dc_setting_set_func setting_func;
    const char *name;
    int required;
    int val;
    const char *env_key;
    const void *(*read_from_string)(const struct dc_posix_env *env, const char *str);
    const char *config_key;
    const void *(*read_from_config)(const struct dc_posix_env *env, config_setting_t *item);
    const void *default_value;
};

struct dc_opt_settings
{
    struct dc_application_settings parent;
    struct options *opts;
    const char *flags;
    const char *env_prefix;
};


void dc_options_set_path(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type);
void dc_options_set_bool(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type);
void dc_options_set_uint16(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type);
const void *dc_string_from_string(const struct dc_posix_env *env, const char *str);
const void *dc_flag_from_string(const struct dc_posix_env *env, const char *str);
const void *dc_uint16_from_string(const struct dc_posix_env *env, const char *str);


#endif // LIBDC_APPLICATION_OPTIONS_H
