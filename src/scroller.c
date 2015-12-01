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
#include "conf.h"
#include "grid.h"
#include "log.h"
#include "page.h"
#include "scroller.h"

#define PRIVATE_SCROLLER_IS_SCROLLING "p_is_sc"
#define PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST "pdkec"



struct _event_cb {
	int event_type;
	void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data);
	void *user_data;
};
typedef struct _event_cb event_cb_s;



int _scroller_is_scrolling(Evas_Object *scroller)
{
	retv_if(!scroller, 0);
	return (size_t)evas_object_data_get(scroller, PRIVATE_SCROLLER_IS_SCROLLING);
}



static void __anim_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start the scroller(%p) animation", scroller);
	evas_object_data_set(scroller, PRIVATE_SCROLLER_IS_SCROLLING, (void *)1);
}



static void __anim_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("stop the scroller(%p) animation", scroller);
	evas_object_data_del(scroller, PRIVATE_SCROLLER_IS_SCROLLING);
}



static void __drag_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start to drag the scroller(%p)", scroller);
}



static void __drag_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("stop to drag the scroller(%p) animation", scroller);
}



static void __scroll_cb(void *data, Evas_Object *scroller, void *event_info)
{
	struct appdata *ad = data;
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	ret_if(!ad);
	ret_if(!scroller);

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	ret_if(!event_cb_list);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (SCROLLER_EVENT_TYPE_SCROLL == event_cb_info->event_type) {
			if (event_cb_info->event_cb) {
				event_cb_info->event_cb(scroller, SCROLLER_EVENT_TYPE_SCROLL, NULL, event_cb_info->user_data);
			}
		}
	}
}



Evas_Object *_scroller_create(Evas_Object *layout, struct appdata *ad)
{
	Evas_Object *box = NULL;
	Evas_Object *scroller = NULL;

	retv_if(!layout, NULL);
	retv_if(!ad, NULL);

	scroller = elm_scroller_add(layout);
	retv_if(!scroller, NULL);

	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set(scroller, 1, 1);
	elm_scroller_content_min_limit(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_single_direction_set(scroller, ELM_SCROLLER_SINGLE_DIRECTION_HARD);

	elm_scroller_page_size_set(scroller, ad->layout_width, -1);

	elm_object_style_set(scroller, "effect");
	evas_object_show(scroller);
	elm_object_scroll_lock_y_set(scroller, EINA_TRUE);
	evas_object_smart_callback_add(scroller, "scroll,anim,start", __anim_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,anim,stop", __anim_stop_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,drag,start", __drag_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,drag,stop", __drag_stop_cb, ad);
	evas_object_smart_callback_add(scroller, "scroll", __scroll_cb, ad);

	box = elm_box_add(scroller);
	goto_if(!box, ERROR);

	elm_box_horizontal_set(box, EINA_TRUE);
	elm_box_align_set(box, 0.5, 0.5);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(box);

	elm_object_content_set(scroller, box);

	return scroller;

ERROR:
	if (scroller) {
		evas_object_del(scroller);
	}
	return NULL;
}



void _scroller_destroy(Evas_Object *scroller)
{
	Evas_Object *box = NULL;
	Eina_List *list = NULL;
	ret_if(!scroller);

	box = elm_object_content_unset(scroller);
	if (box) {
		list = elm_box_children_get(box);
		if (list) {
			_scroller_remove_list(scroller, list);
		}
		evas_object_del(box);
	}

	evas_object_del(scroller);
}



void _scroller_append_page(Evas_Object *scroller, Evas_Object *page)
{
	Evas_Object *box = NULL;
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	box = elm_object_content_get(scroller);
	ret_if(!box);

	elm_box_pack_end(box, page);

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	ret_if(!event_cb_list);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (SCROLLER_EVENT_TYPE_APPEND_PAGE == event_cb_info->event_type) {
			if (event_cb_info->event_cb) {
				event_cb_info->event_cb(scroller, SCROLLER_EVENT_TYPE_APPEND_PAGE, NULL, event_cb_info->user_data);
			}
		}
	}
}



void _scroller_remove_page(Evas_Object *scroller, Evas_Object *page)
{
	Evas_Object *box = NULL;
	Evas_Object *tmp = NULL;
	Eina_List *list = NULL;
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	int is_page_exist = 0;

	box = elm_object_content_get(scroller);
	ret_if(!box);

	list = elm_box_children_get(box);
	EINA_LIST_FREE(list, tmp) {
		continue_if(!tmp);
		if (page == tmp) {
			is_page_exist = 1;
		}
	}

	if (!is_page_exist) {
		_D("No page to remove");
		return;
	}

	elm_box_unpack(box, page);

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	ret_if(!event_cb_list);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (SCROLLER_EVENT_TYPE_REMOVE_PAGE == event_cb_info->event_type) {
			if (event_cb_info->event_cb) {
				event_cb_info->event_cb(scroller, SCROLLER_EVENT_TYPE_REMOVE_PAGE, NULL, event_cb_info->user_data);
			}
		}
	}
}

static int __get_selected_page(struct appdata *ad)
{
	if (ad && ad->selected_index > -1)
		return ad->selected_index / ad->item_per_row;

	return -1;
}

static int __get_selected_grid(struct appdata *ad)
{
	if (ad && ad->selected_index > -1)
		return ad->selected_index % ad->item_per_row;

	return -1;
}

static void __realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	int idx = 0;

	idx = elm_gengrid_item_index_get(event_info);
	_D("realized: %d", idx);

	if (__get_selected_grid(ad) == idx - 1) {
		ad->selected = NULL; /* to ignore launch */
		elm_gengrid_item_selected_set(event_info, EINA_TRUE);
		evas_object_smart_callback_del(obj, "realized", __realized_cb);

		if (ad->selected_index > -1) {
			_E("set page to %d", __get_selected_page(ad));
			elm_scroller_page_show(ad->scroller, __get_selected_page(ad), 0);
	        }
	}
}

void _scroller_append_list(Evas_Object *scroller, Eina_List *list, int page_width, int page_height, int item_per_row, struct appdata *ad)
{
	Evas_Object *page = NULL;
	Evas_Object *grid = NULL;
	int count = 0;
	int i;

	ret_if(!scroller);
	ret_if(!list);

	/* We'll implement this part */
	count = eina_list_count(list);
	if (!count) {
		_D("no apps");
		return;
	}

	_D("list count is %d", count);

	for (i = 0; i < count; i++) {
		Evas_Object *item = NULL;
		if (i % item_per_row == 0) {
			page = _page_create(scroller, page_width, page_height, count, item_per_row);
			ret_if(!page);
			_scroller_append_page(scroller, page);
			grid = _grid_create(page);
			ret_if(!grid);
			elm_object_part_content_set(page, "grid", grid);
			if ((ad->selected_index > -1) &&
					(ad->selected_index / ad->item_per_row) == (i / ad->item_per_row)) {
				/* rotation changed */
				_D("realized callback adeed:%d", i);
				evas_object_smart_callback_add(grid, "realized", __realized_cb, ad);
			}
		}
		item = _grid_append_item(grid, eina_list_nth(list, i));
		ret_if(!item);
	}
}

void _scroller_remove_list(Evas_Object *scroller, Eina_List *list)
{
	Evas_Object *page = NULL;
	Evas_Object *grid = NULL;

	ret_if(!scroller);
	ret_if(!list);

	EINA_LIST_FREE(list, page) {
		grid = elm_object_part_content_unset(page, "grid");
		continue_if(!grid);
		_grid_destroy(grid);
		_page_destroy(page);
	}
}



int _scroller_get_region_index(Evas_Object *scroller)
{
	int index = 0;
	int x = 0;
	int page_w = 0;

	retv_if(!scroller, 0);

	elm_scroller_region_get(scroller, &x, NULL, NULL, NULL);
	elm_scroller_page_size_get(scroller, &page_w, NULL);

	index = x / page_w;
	x = x % page_w;
	if (x > (page_w / 2)) {
		index ++;
	}

	return index;
}



unsigned int _scroller_count(Evas_Object *scroller)
{
	Evas_Object *box = NULL;
	Eina_List *list = NULL;
	int count = 0;

	retv_if(!scroller, 0);

	box = elm_object_content_get(scroller);
	retv_if(!box, 0);

	list = elm_box_children_get(box);
	retv_if(!list, 0);

	count = eina_list_count(list);
	eina_list_free(list);

	return count;
}



void _scroller_bring_in_page(Evas_Object *scroller, Evas_Object *page)
{
	Evas_Object *box = NULL;
	Evas_Object *tmp = NULL;
	Eina_List *list = NULL;
	int index = 0;
	int exist = 1;

	ret_if(!scroller);
	ret_if(!page);

	box = elm_object_content_get(scroller);
	ret_if(!box);

	list = elm_box_children_get(box);
	ret_if(!list);

	EINA_LIST_FREE(list, tmp) {
		continue_if(!tmp);
		if (page == tmp) {
			exist = 0;
		}
		index += exist;
	}

	if (exist) {
		index = 0;
	}

	elm_scroller_page_bring_in(scroller, index, 0);
}



void _scroller_resize(Evas_Object *scroller, int width, int height)
{
	Evas_Object *box = NULL;
	Evas_Object *page = NULL;
	Eina_List *list = NULL;

	ret_if(!scroller);

	elm_scroller_page_size_set(scroller, width, height);
	evas_object_size_hint_min_set(scroller, width, height);
	evas_object_size_hint_max_set(scroller, width, height);

	box = elm_object_content_get(scroller);
	ret_if(!box);

	evas_object_size_hint_min_set(box, width, height);
	evas_object_size_hint_max_set(box, width, height);

	list = elm_box_children_get(box);
	ret_if(!list);

	EINA_LIST_FREE(list, page) {
		_page_resize(page, width, height);
	}
}



int _scroller_register_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data), void *user_data)
{
	Eina_List *event_cb_list = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!scroller, -1);
	retv_if(event_type <= SCROLLER_EVENT_TYPE_INVALID, -1);
	retv_if(event_type >= SCROLLER_EVENT_TYPE_MAX, -1);
	retv_if(!event_cb, -1);

	event_cb_info = calloc(1, sizeof(event_cb_s));
	retv_if(!event_cb_info, -1);

	event_cb_info->event_type = event_type;
	event_cb_info->event_cb = event_cb;
	event_cb_info->user_data = user_data;

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	event_cb_list = eina_list_append(event_cb_list, event_cb_info);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return 0;
}



int _scroller_unregister_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data))
{
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!scroller, -1);
	retv_if(event_type <= SCROLLER_EVENT_TYPE_INVALID, -1);
	retv_if(event_type >= SCROLLER_EVENT_TYPE_MAX, -1);
	retv_if(!event_cb, -1);

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	retv_if(!event_cb_list, -1);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (event_cb_info->event_type == event_type
			&& event_cb_info->event_cb == event_cb)
		{
			event_cb_list = eina_list_remove(event_cb_list, event_cb_info);
			break;
		}
	}

	evas_object_data_set(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return 0;
}



// End of file
