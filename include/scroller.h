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


#ifndef __TIZEN_APP_SELECTOR_SCROLLER_H__
#define __TIZEN_APP_SELECTOR_SCROLLER_H__

#include <Elementary.h>
#include "type.h"

typedef enum {
	SCROLLER_EVENT_TYPE_INVALID = 0,
	SCROLLER_EVENT_TYPE_SCROLL,
	SCROLLER_EVENT_TYPE_APPEND_PAGE,
	SCROLLER_EVENT_TYPE_REMOVE_PAGE,
	SCROLLER_EVENT_TYPE_MAX,
} scroller_event_type_e;

extern Evas_Object *_scroller_create(Evas_Object *layout, struct appdata *ad);
extern void _scroller_destroy(Evas_Object *scroller);

extern void _scroller_append_page(Evas_Object *scroller, Evas_Object *page);
extern void _scroller_remove_page(Evas_Object *scroller, Evas_Object *page);

extern void _scroller_append_list(Evas_Object *scroller, Eina_List *list, int page_width, int page_height, int item_per_row, struct appdata *ad);
extern void _scroller_remove_list(Evas_Object *scroller, Eina_List *list);

extern void _scroller_bring_in_page(Evas_Object *scroller, Evas_Object *page);
extern int _scroller_get_region_index(Evas_Object *scroller);
extern unsigned int _scroller_count(Evas_Object *scroller);

extern int _scroller_is_scrolling(Evas_Object *scroller);
extern void _scroller_resize(Evas_Object *scroller, int width, int height);

extern int _scroller_register_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *data), void *data);
extern int _scroller_unregister_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *data));

#endif /* __TIZEN_APP_SELECTOR_SCROLLER_H__ */
