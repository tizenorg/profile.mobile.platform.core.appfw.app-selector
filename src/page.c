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

#include <Elementary.h>

#include "conf.h"
#include "log.h"
#include "page.h"



#define PAGE_EDJE_FILE EDJEDIR"/page.edj"
Evas_Object *_page_create(Evas_Object *scroller, int page_width, int page_height, int items, int item_per_row)
{
	Evas_Object *page = NULL;
	Evas_Object *page_bg = NULL;
	Evas *e = NULL;

	page = elm_layout_add(scroller);
	retv_if(!page, NULL);
	elm_layout_file_set(page, PAGE_EDJE_FILE, "page");

	e = evas_object_evas_get(scroller);
	goto_if(!e, ERROR);

	page_bg = evas_object_rectangle_add(e);
	goto_if(!page_bg, ERROR);
	evas_object_size_hint_weight_set(page_bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(page_bg, page_width, page_height);
	evas_object_size_hint_max_set(page_bg, page_width, page_height);
	evas_object_color_set(page_bg, 0, 0, 0, 0);
	evas_object_repeat_events_set(page_bg, EINA_TRUE);
	evas_object_show(page_bg);
	elm_object_part_content_set(page, "bg", page_bg);

	evas_object_show(page);
	return page;

ERROR:
	_page_destroy(page);
	return NULL;
}



void _page_destroy(Evas_Object *page)
{
	Evas_Object *bg = NULL;

	ret_if(!page);

	bg = elm_object_part_content_unset(page, "bg");
	if (bg) {
		evas_object_del(bg);
	}

	evas_object_del(page);
}



void _page_resize(Evas_Object *page, int width, int height)
{
	Evas_Object *bg = NULL;

	ret_if(!page);

	bg = elm_object_part_content_get(page, "bg");
	ret_if(!bg);

	evas_object_size_hint_min_set(bg, width, height);
	evas_object_size_hint_max_set(bg, width, height);
}



//End of file
