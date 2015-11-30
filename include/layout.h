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


#ifndef __TIZEN_APP_SELECTOR_LAYOUT_H__
#define __TIZEN_APP_SELECTOR_LAYOUT_H__

#include <Elementary.h>
#include "type.h"

extern Evas_Object *_layout_create(struct appdata *ad);
extern void _layout_destroy(Evas_Object *layout);

extern void _layout_resize(Evas_Object *layout, int width, int height);

#endif /* __TIZEN_APP_SELECTOR_LAYOUT_H__ */
