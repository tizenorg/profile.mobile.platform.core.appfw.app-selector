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

#include <pkgmgr-info.h>
#include "conf.h"
#include "log.h"
#include "grid.h"
#include "util.h"
#include "popup.h"

#define PRIVATE_DATA_KEY_ITEM_INFO "pdkii"
#define DEFAULT_ICON "/usr/share/icons/A01-1_icon_Menu.png"

static item_s *selected_item = NULL;

static struct {
	Elm_Gengrid_Item_Class *gic;
	char *default_icon;
} grid_info = {
	.gic = NULL,
	.default_icon = DEFAULT_ICON,
};

static char *__text_get(void *data, Evas_Object *obj, const char *part)
{
	item_s *info = data;
	retv_if(!info, NULL);

	retv_if(!info->name, NULL);

	if (!strcmp(part, "elm.text")) {
		return strdup(dgettext(PACKAGE, info->name));
	}

	return NULL;
}


static Evas_Object *__add_icon(Evas_Object *parent, const char *file)
{
	const char *real_icon_file = NULL;
	Evas_Object *icon = NULL;

	real_icon_file = file;
	if (access(real_icon_file, R_OK) != 0) {
		_E("Failed to access an icon(%s)", real_icon_file);
		real_icon_file = DEFAULT_ICON;
	}

	icon = elm_icon_add(parent);
	retv_if(!icon, NULL);

	if (elm_image_file_set(icon, real_icon_file, NULL) == EINA_FALSE) {
		_E("Icon file is not accessible (%s)", real_icon_file);
		evas_object_del(icon);
		return NULL;
	}

	evas_object_size_hint_min_set(icon, ELM_SCALE_SIZE(ITEM_ICON_WIDTH), ELM_SCALE_SIZE(ITEM_ICON_HEIGHT));
	evas_object_size_hint_max_set(icon, ELM_SCALE_SIZE(ITEM_ICON_WIDTH), ELM_SCALE_SIZE(ITEM_ICON_HEIGHT));

	elm_image_preload_disabled_set(icon, EINA_TRUE);
	elm_image_smooth_set(icon, EINA_TRUE);
	elm_image_no_scale_set(icon, EINA_FALSE);
	evas_object_show(icon);

	return icon;
}


static Evas_Object *__content_get(void *data, Evas_Object  *obj, const char *part)
{
	item_s *info = data;

	retv_if(!info, NULL);

	if (!strcmp(part, "elm.swallow.end")) {
		Evas_Object *bg = evas_object_rectangle_add(evas_object_evas_get(obj));
		retv_if(!bg, NULL);

		evas_object_color_set(bg, 0, 0, 0, 0);
		evas_object_show(bg);
		return bg;
	} else if (!strcmp(part, "elm.swallow.icon")) {
		retv_if(!info->icon, NULL);
		return __add_icon(obj, info->icon);
	}

	return NULL;
}


static void __del(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
	evas_object_data_del(obj, PRIVATE_DATA_KEY_ITEM_INFO);
}


static void __item_selected(void *data, Evas_Object *obj, void *event_info)
{
	item_s *item_info = data;
	Elm_Object_Item *selected = NULL;

	ret_if(!item_info);
	selected = elm_gengrid_selected_item_get(obj);
	ret_if(!selected);
	if (item_info->extra || item_info->ad->selected == item_info) {
		elm_gengrid_item_selected_set(selected, EINA_FALSE);
		_util_launch_selected_app(item_info);
		return;
	} else {
		if (selected_item == NULL) {
			popup_button_disabled_set(EINA_FALSE, item_info->ad);
		}

		item_info->ad->selected_index = item_info->idx;
		_D("selected index updated to %d", item_info->idx);
		item_info->ad->selected = item_info;
		if (selected_item && selected_item->grid_item)
			elm_object_item_signal_emit(selected_item->grid_item, "elm,select,disabled", "");

		selected_item = item_info;
		if (selected_item && selected_item->grid_item)
			elm_object_item_signal_emit(selected_item->grid_item, "elm,select,enabled", "");
	}
}


static void __lang_changed_cb(void *data, Evas_Object *grid, void *event_info)
{
	Elm_Object_Item *it = NULL;
	int ret = 0;
	char *name = NULL;
	ret_if(!grid);

	it = elm_gengrid_first_item_get(grid);
	while (it) {
		pkgmgrinfo_appinfo_h handle = NULL;
		item_s *item_info = NULL;

		item_info = evas_object_data_get(it, PRIVATE_DATA_KEY_ITEM_INFO);
		goto_if(!item_info, next);

		ret = pkgmgrinfo_appinfo_get_appinfo(item_info->appid, &handle);
		goto_if(ret != PMINFO_R_OK || handle == NULL, next);

		ret = pkgmgrinfo_appinfo_get_label(handle, &name);
		if (ret != PMINFO_R_OK || name == NULL) {
			_E("failed to get appinfo label");
			goto next;
		}

		if (item_info->name)
			free(item_info->name);

		item_info->name = strdup(name);
		elm_object_item_part_text_set(it, "elm.text", item_info->name);
		elm_gengrid_item_update(it);

next:
		if (handle)
			pkgmgrinfo_appinfo_destroy_appinfo(handle);

		it = elm_gengrid_item_next_get(it);
	}
}


static void __grid_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Coord w = 0, h = 0;
	Evas_Object *grid = (Evas_Object *)data;

	if (!grid) {
		_E("wrong arguments");
		return;
	}

	evas_object_geometry_get(grid, NULL, NULL, &w, &h);
	_D("grid resized to w:%d h:%d", w, h);
	elm_gengrid_item_size_set(grid, w / ITEM_CNT, ELM_SCALE_SIZE(ITEM_HEIGHT));
}

Evas_Object *_grid_create(Evas_Object *page)
{
	Evas_Object *grid = NULL;

	retv_if(!page, NULL);
	selected_item = NULL;

	grid = elm_gengrid_add(page);
	goto_if(!grid, ERROR);

	evas_object_event_callback_add(grid, EVAS_CALLBACK_RESIZE, __grid_resize_cb, grid);
	evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_gengrid_align_set(grid, 0.0, 0.5);
	elm_gengrid_horizontal_set(grid, EINA_FALSE);
	elm_gengrid_multi_select_set(grid, EINA_FALSE);
	elm_gengrid_select_mode_set(grid, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_object_style_set(grid, "popup");

	grid_info.gic = elm_gengrid_item_class_new();
	goto_if(!grid_info.gic, ERROR);
	grid_info.gic->func.text_get = __text_get;
	grid_info.gic->func.content_get = __content_get;
	grid_info.gic->func.state_get = NULL;
	grid_info.gic->func.del = __del;
	grid_info.gic->item_style = "default";

	evas_object_smart_callback_add(grid, "language,changed", __lang_changed_cb, NULL);

	elm_scroller_movement_block_set(grid, ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);
	evas_object_show(grid);

	return grid;

ERROR:
	_grid_destroy(grid);
	return NULL;
}


void _grid_destroy(Evas_Object *grid)
{
	ret_if(!grid);

	evas_object_del(grid);
}


Elm_Object_Item *_grid_append_item(Evas_Object *grid, item_s *item_info)
{
	Elm_Object_Item *item = NULL;

	retv_if(!grid, NULL);
	retv_if(!item_info, NULL);

	retv_if(!grid_info.gic, NULL);

	item = elm_gengrid_item_append(grid, grid_info.gic, item_info, __item_selected, item_info);
	retv_if(!item, NULL);
	evas_object_data_set(item, PRIVATE_DATA_KEY_ITEM_INFO, item_info);
	if (item_info->name) {
		_D("grid append item: %s", item_info->name);
	}
	item_info->grid_item = item;
	elm_gengrid_item_show(item, ELM_GENGRID_ITEM_SCROLLTO_NONE);
	elm_gengrid_item_update(item);

	return item;
}


void _grid_remove_item(Evas_Object *grid, item_s *item_info)
{
	Elm_Object_Item *item = NULL;

	ret_if(!grid);
	ret_if(!item_info);

	item = item_info->grid_item;

	ret_if(!item);
	evas_object_data_del(item, PRIVATE_DATA_KEY_ITEM_INFO);
	elm_object_item_del(item);
	item_info->grid_item = NULL;
}

int _grid_count_item(Evas_Object *grid)
{
	int count = 0;

	retv_if(!grid, 0);

	count = elm_gengrid_items_count(grid);

	return count;
}

//End of file

