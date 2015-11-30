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

#include "type.h"
#include "log.h"
#include "page.h"
#include "index.h"
#include "scroller.h"
#include "conf.h"

#define FILE_LAYOUT_EDJ EDJEDIR"/layout.edj"
#define GROUP_LAYOUT "layout"
#define DATA_KEY_APP_SELECTOR_INFO "__dkasi__"

static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *layout = obj;

	int x, y, w, h;

	ret_if(!layout);

	evas_object_geometry_get(layout, &x, &y, &w, &h);
	_D("%s resize(%d, %d, %d, %d)", data, x, y, w, h);
}



static Evas_Object *__layout_create_rect(Evas_Object *layout, int width, int height)
{
	Evas_Object *rect = NULL;

	retv_if(!layout, NULL);

	_D("Create a rectangle for app selector");

	rect = evas_object_rectangle_add(evas_object_evas_get(layout));
	retv_if(!rect, NULL);

	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(rect, width, height);
	evas_object_color_set(rect, 0, 0, 0, 0);
	evas_object_show(rect);

	elm_object_part_content_set(layout, "layout", rect);
	evas_object_event_callback_add(rect, EVAS_CALLBACK_RESIZE, __resize_cb, "layout");

	return rect;
}



static void __layout_destroy_rect(Evas_Object *layout)
{
	Evas_Object *rect = NULL;

	ret_if(!layout);

	rect = elm_object_part_content_unset(layout, "layout");
	ret_if(!rect);

	evas_object_del(rect);
}



static void _scroll_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	size_t region_index = 0;

	ret_if(!index);

	region_index = _scroller_get_region_index(scroller);
	_index_bring_in(index, region_index);

	_D("Scroller region index is %d", region_index);
}



static void _append_page_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	unsigned int count = 0;

	ret_if(!index);

	count = _scroller_count(scroller);
	_index_update(index, count);

	_D("Index was updated as %d", count);
}



static void _remove_page_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	unsigned int count = 0;

	ret_if(!index);

	count = _scroller_count(scroller);
	_index_update(index, count);

	_D("Index was updated as %d", count);
}


Evas_Object *_layout_create(struct appdata *ad)
{
	Evas_Object *layout = NULL;
	Evas_Object *edje = NULL;
	int list_length = eina_list_count(ad->list);

	retv_if(!ad, NULL);

	layout = elm_layout_add(ad->popup);
	retv_if(!layout, NULL);

	elm_layout_file_set(layout, FILE_LAYOUT_EDJ, GROUP_LAYOUT);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_move(layout, 0, 0);
	evas_object_resize(layout, ad->layout_width, ad->layout_height);

	elm_object_part_content_set(ad->popup, "elm.swallow.content", layout);

	evas_object_show(layout);

	ad->layout_rect = __layout_create_rect(layout, ad->layout_width, ad->layout_height);

	goto_if(!ad->layout_rect, ERROR);

	evas_object_data_set(layout, DATA_KEY_APP_SELECTOR_INFO, ad);

	if (list_length > ad->item_per_row) {
		ad->index = _index_create(layout, 0);
		goto_if(!ad->index, ERROR);
		elm_object_part_content_set(layout, "index", ad->index);
		elm_layout_signal_emit(layout, "enable_index", "");
	} else {
		_E("no index");
		elm_object_part_content_unset(layout, "index");
		elm_layout_signal_emit(layout, "disable_index", "");
	}

	ad->scroller = _scroller_create(layout, ad);
	goto_if(!ad->scroller, ERROR);
	elm_object_part_content_set(layout, "scroller", ad->scroller);

	if (_scroller_register_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb, ad->index) < 0) {
		_E("cannot register the scroller event");
	}
	if (_scroller_register_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_APPEND_PAGE, _append_page_cb, ad->index) < 0) {
		_E("cannot register the scroller event");
	}
	if (_scroller_register_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_REMOVE_PAGE, _remove_page_cb, ad->index) < 0) {
		_E("cannot register the scroller event");
	}

	_scroller_append_list(ad->scroller
		, ad->list
		, ad->layout_width
		, ad->layout_height - (ad->index ? ELM_SCALE_SIZE(INDEX_HEIGHT) : 0), ad->item_per_row, ad);

	edje = elm_layout_edje_get(layout);
	goto_if(!edje, ERROR);

	edje_object_message_signal_process(edje);

	return layout;

ERROR:
	if (ad->index) _index_destroy(ad->index);
	if (ad->scroller) _scroller_destroy(ad->scroller);
	if (layout) {
		__layout_destroy_rect(layout);
		evas_object_del(layout);
	}
	return NULL;
}



void _layout_destroy(Evas_Object *layout)
{
	struct appdata *ad = NULL;

	ad = evas_object_data_del(layout, DATA_KEY_APP_SELECTOR_INFO);
	ret_if(!ad);

	if (ad->scroller) {
		_scroller_unregister_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb);
		_scroller_unregister_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_APPEND_PAGE, _append_page_cb);
		_scroller_unregister_event_cb(ad->scroller, SCROLLER_EVENT_TYPE_REMOVE_PAGE, _remove_page_cb);
		_scroller_destroy(ad->scroller);
		ad->scroller = NULL;
	}

	if (ad->index) {
		_index_destroy(ad->index);
		ad->index = NULL;
	}

	if (ad->layout) {
		__layout_destroy_rect(layout);
		ad->layout_rect = NULL;
		evas_object_del(layout);
		ad->layout = NULL;
	}
}



void _layout_resize(Evas_Object *layout, int width, int height)
{
	ret_if(!layout);

	evas_object_size_hint_min_set(layout, width, height);
	evas_object_size_hint_max_set(layout, width, height);
	evas_object_resize(layout, width, height);
	evas_object_move(layout, 0, 0);
}


