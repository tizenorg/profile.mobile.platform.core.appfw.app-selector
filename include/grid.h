/*
 * Copyright 2015  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://floralicense.org/license

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __TIZEN_APP_SELECTOR_GRID_H__
#define __TIZEN_APP_SELECTOR_GRID_H__

#include <Elementary.h>
#include "type.h"

extern Evas_Object *_grid_create(Evas_Object *page);
extern void _grid_destroy(Evas_Object *item);

extern Elm_Object_Item *_grid_append_item(Evas_Object *grid, item_s *item_info);
extern void _grid_remove_item(Evas_Object *grid, item_s *item_info);

extern int _grid_count_item(Evas_Object *grid);

#endif /* __TIZEN_APP_SELECTOR_GRID_H__ */
