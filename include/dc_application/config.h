#ifndef LIBDC_APPLICATION_CONFIG_H
#define LIBDC_APPLICATION_CONFIG_H

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

#include "application.h"
#include <dc_env/env.h>
#include <libconfig.h>

int dc_default_load_config(const struct dc_env *env, struct dc_error *err,
                           struct dc_application_settings *settings);

const void *dc_string_from_config(const struct dc_env *env,
                                  struct dc_error *err, config_setting_t *item);

const void *dc_flag_from_config(const struct dc_env *env,
                                struct dc_error *err, config_setting_t *item);

const void *dc_uint16_from_config(const struct dc_env *env,
                                  struct dc_error *err, config_setting_t *item);

#endif // LIBDC_APPLICATION_CONFIG_H
