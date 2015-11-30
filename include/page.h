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


#ifndef __TIZEN_APP_SELECTOR_PAGE_H__
#define __TIZEN_APP_SELECTOR_PAGE_H__

#include <Elementary.h>

extern Evas_Object *_page_create(Evas_Object *scroller, int page_width, int page_height, int items, int item_per_row);
extern void _page_destroy(Evas_Object *page);

extern void _page_resize(Evas_Object *page, int width, int height);

#endif /* __TIZEN_APP_SELECTOR_PAGE_H__ */
