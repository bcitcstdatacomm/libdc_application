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


#include "settings.h"
#include <stdlib.h>
#include <string.h>
#include <dc_util/path.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
struct dc_setting_path
{
    struct dc_setting parent;
    char *path;
};

struct dc_setting_bool
{
    struct dc_setting parent;
    bool value;
};
#pragma GCC diagnostic pop

bool dc_setting_is_set(struct dc_setting *setting)
{
    return setting->type != DC_SETTING_NONE;
}

struct dc_setting_path *dc_setting_path_create(void)
{
    struct dc_setting_path *setting;

    setting = malloc(sizeof(struct dc_setting_path));
    setting->parent.type = DC_SETTING_NONE;
    setting->path        = NULL;

    return setting;
}

void dc_setting_path_destroy(struct dc_setting_path **psetting)
{
    free((*psetting)->path);
    memset(*psetting, 0, sizeof(struct dc_setting_path));
    free(*psetting);
    *psetting = NULL;
}

bool dc_setting_path_set(struct dc_setting_path *setting, const char *value, dc_setting_type type)
{
    bool ret_val;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        expand_path(&setting->path, value);
        setting->parent.type = type;
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

const char *dc_setting_path_get(struct dc_setting_path *setting)
{
    return setting->path;
}

struct dc_setting_bool *dc_setting_bool_create(void)
{
    struct dc_setting_bool *setting;

    setting = malloc(sizeof(struct dc_setting_bool));
    setting->parent.type = DC_SETTING_NONE;
    setting->value       = false;

    return setting;
}

void dc_setting_bool_destroy(struct dc_setting_bool **psetting)
{
    memset(*psetting, 0, sizeof(struct dc_setting_path));
    free(*psetting);
    *psetting = NULL;
}

bool dc_setting_bool_set(struct dc_setting_bool *setting, bool value, dc_setting_type type)
{
    bool ret_val;

    if(setting->parent.type == DC_SETTING_NONE)
    {
        setting->parent.type = type;
        setting->value = value;
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

bool dc_setting_bool_get(struct dc_setting_bool *setting)
{
    return setting->value;
}
