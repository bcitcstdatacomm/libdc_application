#include "options.h"
#include <dc_posix/stdlib.h>
#include <dc_util/types.h>


void dc_options_set_path(const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    dc_setting_path_set(env, (struct dc_setting_path *)setting, (const char *)value, type);
}

void dc_options_set_bool(__attribute__((unused)) const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const bool *pbool;

    pbool = value;
    dc_setting_bool_set((struct dc_setting_bool *)setting, *pbool, type);
}

void dc_options_set_uint16(__attribute__((unused)) const struct dc_posix_env *env, struct dc_setting *setting, const void *value, dc_setting_type type)
{
    const uint16_t *punit16;

    punit16 = value;
    dc_setting_uint16_set((struct dc_setting_uint16 *)setting, *punit16, type);
}

const void *dc_string_from_string(__attribute__((unused)) const struct dc_posix_env *env, const char *str)
{
    return str;
}

const void *dc_flag_from_string(__attribute__((unused)) const struct dc_posix_env *env, __attribute__((unused))  const char *str)
{
    static bool value = true;

    return &value;
}

const void *dc_uint16_from_string(const struct dc_posix_env *env, const char *str)
{
    int       err;
    uint16_t *value;

    value = dc_malloc(env, &err, sizeof(uint16_t));

    if(value == NULL)
    {
    }

    *value = dc_uint16_from_str(env, &err, str, 10);

    return value;
}
