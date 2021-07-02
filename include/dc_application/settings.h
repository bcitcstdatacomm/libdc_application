#ifndef LIBDC_APPLICATION_SETTINGS_H
#define LIBDC_APPLICATION_SETTINGS_H


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


#include <stdint.h>
#include <stdbool.h>
#include <dc_posix/posix_env.h>


// TODO: move these to the .c file and change the arguments to the functins
// on create set it to NONE and after that chnage it to SET and get rid of all
// toe other values. Really it can be a bool - it is either set or it is not.
typedef enum
{
    DC_SETTING_NONE = -1,
    DC_SETTING_DEFAULT,
    DC_SETTING_COMMAND_LINE,
    DC_SETTING_ENVIRONMENT,
    DC_SETTING_CONFIG,
}
dc_setting_type;

struct dc_setting
{
    dc_setting_type type;
};

struct dc_setting_path;
struct dc_setting_bool;
struct dc_setting_uint16;


/**
 *
 * @param setting
 * @return
 */
bool dc_setting_is_set(struct dc_setting *setting);

/**
 *
 * @return
 */
struct dc_setting_path *dc_setting_path_create(void);

/**
 *
 * @param psetting
 */
void dc_setting_path_destroy(struct dc_setting_path **psetting);

/**
 *
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_path_set(const struct dc_posix_env *env, struct dc_setting_path *setting, const char *value, dc_setting_type type);

/**
 *
 * @param setting
 * @return
 */
const char *dc_setting_path_get(struct dc_setting_path *setting);

/**
 *
 * @return
 */
struct dc_setting_bool *dc_setting_bool_create(void);

/**
 *
 * @param psetting
 */
void dc_setting_bool_destroy(struct dc_setting_bool **psetting);

/**
 *
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_bool_set(struct dc_setting_bool *setting, bool value, dc_setting_type type);

/**
 *
 * @param setting
 * @return
 */
bool dc_setting_bool_get(struct dc_setting_bool *setting);

/**
 *
 * @return
 */
struct dc_setting_uint16 *dc_setting_uint16_create(void);

/**
 *
 * @param psetting
 */
void dc_setting_uint16_destroy(struct dc_setting_uint16 **psetting);

/**
 *
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_uint16_set(struct dc_setting_uint16 *setting, uint16_t value, dc_setting_type type);

/**
 *
 * @param setting
 * @return
 */
uint16_t dc_setting_uint16_get(struct dc_setting_uint16 *setting);

#endif // LIBDC_APPLICATION_SETTINGS_H
