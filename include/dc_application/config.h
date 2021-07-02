#ifndef LIBDC_APPLICATION_CONFIG_H
#define LIBDC_APPLICATION_CONFIG_H


#include "application.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#include <libconfig.h>
#pragma GCC diagnostic pop


int dc_default_load_config(struct dc_application_settings *settings);
const void *dc_string_from_config(const struct dc_posix_env *env, config_setting_t *item);
const void *dc_flag_from_config(const struct dc_posix_env *env, config_setting_t *item);
const void *dc_uint16_from_config(const struct dc_posix_env *env, config_setting_t *item);



#endif // LIBDC_APPLICATION_CONFIG_H
