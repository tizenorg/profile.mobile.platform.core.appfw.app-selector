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


#ifndef __TIZEN_APP_SELECTOR_INDEX_H__
#define __TIZEN_APP_SELECTOR_INDEX_H__

#include <Evas.h>

extern Evas_Object *_index_create(Evas_Object *layout, unsigned int count);
extern void _index_update(Evas_Object *index, unsigned int count);
extern void _index_destroy(Evas_Object *index);

extern void _index_bring_in(Evas_Object *index, size_t idx);

#endif //__TIZEN_APP_SELECTOR_INDEX_H__
