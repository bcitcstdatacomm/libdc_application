#ifndef LIBDC_APPLICATION_SETTINGS_H
#define LIBDC_APPLICATION_SETTINGS_H


/*
 * Copyright 2021-2022 D'Arcy Smith.
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


#include <dc_env/env.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


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
} dc_setting_type;

struct dc_setting
{
    dc_setting_type type;
};

struct dc_setting_string;
struct dc_setting_regex;
struct dc_setting_path;
struct dc_setting_bool;
struct dc_setting_uint16;


/**
 *
 * @param env
 * @param setting
 * @return
 */
bool dc_setting_is_set(const struct dc_env *env,
                       struct dc_setting *setting);


/**
 *
 * @param env
 * @param err
 * @return
 */
struct dc_setting_string *
dc_setting_string_create(const struct dc_env *env, struct dc_error *err);


/**
 *
 * @param env
 * @param psetting
 */
void dc_setting_string_destroy(const struct dc_env *env,
                               struct dc_setting_string **psetting);


/**
 *
 * @param env
 * @param err
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_string_set(const struct dc_env *env, struct dc_error *err,
                           struct dc_setting_string *setting, const char *value,
                           dc_setting_type type);


/**
 *
 * @param env
 * @param setting
 * @return
 */
const char *dc_setting_string_get(const struct dc_env *env,
                                  struct dc_setting_string *setting);


/**
 *
 * @param env
 * @param err
 * @param pattern
 * @return
 */
struct dc_setting_regex *dc_setting_regex_create(const struct dc_env *env,
                                                 struct dc_error *err,
                                                 const char *pattern);


/**
 *
 * @param env
 * @param psetting
 */
void dc_setting_regex_destroy(const struct dc_env *env,
                              struct dc_setting_regex **psetting);


/**
 *
 * @param env
 * @param err
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_regex_set(const struct dc_env *env, struct dc_error *err,
                          struct dc_setting_regex *setting, const char *value,
                          dc_setting_type type);


/**
 *
 * @param env
 * @param setting
 * @return
 */
const char *dc_setting_regex_get(const struct dc_env *env,
                                 struct dc_setting_regex *setting);


/**
 *
 * @param env
 * @param err
 * @return
 */
struct dc_setting_path *dc_setting_path_create(const struct dc_env *env,
                                               struct dc_error *err);


/**
 *
 * @param env
 * @param psetting
 */
void dc_setting_path_destroy(const struct dc_env *env,
                             struct dc_setting_path **psetting);


/**
 *
 * @param env
 * @param err
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_path_set(const struct dc_env *env, struct dc_error *err,
                         struct dc_setting_path *setting, const char *value,
                         dc_setting_type type);


/**
 *
 * @param env
 * @param setting
 * @return
 */
const char *dc_setting_path_get(const struct dc_env *env,
                                struct dc_setting_path *setting);


/**
 *
 * @param env
 * @param err
 * @return
 */
struct dc_setting_bool *dc_setting_bool_create(const struct dc_env *env,
                                               struct dc_error *err);



/**
 *
 * @param env
 * @param psetting
 */
void dc_setting_bool_destroy(const struct dc_env *env,
                             struct dc_setting_bool **psetting);



/**
 *
 * @param env
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_bool_set(const struct dc_env *env,
                         struct dc_setting_bool *setting, bool value,
                         dc_setting_type type);


/**
 *
 * @param env
 * @param setting
 * @return
 */
bool dc_setting_bool_get(const struct dc_env *env,
                         struct dc_setting_bool *setting);


/**
 *
 * @param env
 * @param err
 * @return
 */
struct dc_setting_uint16 *
dc_setting_uint16_create(const struct dc_env *env, struct dc_error *err);


/**
 *
 * @param env
 * @param psetting
 */
void dc_setting_uint16_destroy(const struct dc_env *env,
                               struct dc_setting_uint16 **psetting);


/**
 *
 * @param env
 * @param setting
 * @param value
 * @param type
 * @return
 */
bool dc_setting_uint16_set(const struct dc_env *env,
                           struct dc_setting_uint16 *setting, uint16_t value,
                           dc_setting_type type);


/**
 *
 * @param env
 * @param setting
 * @return
 */
uint16_t dc_setting_uint16_get(const struct dc_env *env,
                               struct dc_setting_uint16 *setting);


#ifdef __cplusplus
}
#endif


#endif // LIBDC_APPLICATION_SETTINGS_H
