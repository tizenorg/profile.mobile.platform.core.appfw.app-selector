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

#include <Evas.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <bundle_internal.h>

#include "util.h"
#include "log.h"
#include "type.h"
#include "layout.h"
#include "conf.h"

#define __ITEM_ROW_CNT 4
#define SYSSTRING "sys_string"

static Evas_Object *always_btn, *once_btn, *ok_btn;
static void __once_btn_response_cb(void *data, Evas_Object * obj, void *event_info);

static void __popup_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	_util_recycle_app(data);
}

static Eina_Bool __unload_info_popup(void *data)
{
	ui_app_exit();

	return ECORE_CALLBACK_CANCEL;
}

int popup_load_info(struct appdata *ad, const char *msg)
{
	Evas_Object *eo = NULL;
	eo = elm_popup_add(ad->win);
	elm_popup_align_set(eo, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	elm_object_text_set(eo, msg);

	ecore_timer_add(2.0, __unload_info_popup, eo);
	eext_object_event_callback_add(eo, EEXT_CALLBACK_BACK, __popup_back_cb, ad);
	evas_object_show(eo);

	return 0;
}

static void __clear_defaults_popup_cb(void *data, Evas_Object * obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	item_s *info = NULL;
	info = ad->selected;
	if(info == NULL) {
		_E("item data is null");
		return;
	}

	_D("__clear_defaults_popup_cb");

	evas_object_smart_callback_del(ok_btn, "clicked", __clear_defaults_popup_cb);
	elm_object_disabled_set(ok_btn, EINA_TRUE);

	_util_set_as_default((void*)info);
	_util_launch_selected_app((void*)info);
}

void __clear_default_popup_text_set(void *data, Evas_Object *obj, void *event_info)
{
	elm_object_text_set(obj, _("IDS_ST_POP_TO_CLEAR_DEFAULT_APPLICATION_SETTINGS_GO_TO_SETTINGS_APPLICATION_MANAGER_DEFAULT_AND_SELECT_THE_APPLICATION_BEING_USED_BY_DEFAULT_MSG"));
}

static void __always_btn_response_cb(void *data, Evas_Object * obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	item_s *info = NULL;
	Evas_Object *eo = NULL;

	info = ad->selected;
	if(info == NULL) {
		_E("item data is null");
		return;
	}

	_D("__always_btn_response_cb. show clear defaults popup");

	evas_object_smart_callback_del(always_btn, "clicked", __always_btn_response_cb);
	evas_object_smart_callback_del(once_btn, "clicked", __once_btn_response_cb);
	elm_object_disabled_set(once_btn, EINA_TRUE);

	eo = elm_popup_add(info->ad->win);
	evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	elm_popup_align_set(eo, ELM_NOTIFY_ALIGN_FILL, 1.0);

	ok_btn = elm_button_add(eo);
	elm_object_part_content_set(eo, "button1", ok_btn);
	evas_object_smart_callback_add(ok_btn, "clicked", __clear_defaults_popup_cb, ad);

	elm_object_domain_translatable_part_text_set(
			eo, "title,text", PACKAGE, "IDS_ST_BODY_CLEAR_DEFAULTS");
	elm_object_domain_translatable_text_set(
			ok_btn, PACKAGE, "IDS_COM_SK_OK");	/* SYSSTRING is used in tizen 2.4 */

	__clear_default_popup_text_set(NULL,eo,NULL);
	evas_object_smart_callback_add(eo, "language,changed", __clear_default_popup_text_set, NULL);

#ifdef USE_POPUP_TIMEOUT
	elm_popup_timeout_set(eo, 7.0);
	evas_object_smart_callback_add(eo, "timeout", __clear_defaults_popup_cb, ad);
#endif

	eext_object_event_callback_add(eo, EEXT_CALLBACK_BACK, __popup_back_cb, ad);

	evas_object_show(eo);
	info->ad->default_popup = eo;
}


static void __once_btn_response_cb(void *data, Evas_Object * obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	item_s *info = NULL;

	info = ad->selected;
	if(info == NULL) {
		_E("item data is null");
		return;
	}

	_D("__once_btn_response_cb");

	evas_object_smart_callback_del(once_btn, "clicked", __once_btn_response_cb);
	evas_object_smart_callback_del(always_btn, "clicked", __always_btn_response_cb);
	elm_object_disabled_set(always_btn, EINA_TRUE);
	elm_object_disabled_set(once_btn, EINA_TRUE);

	_util_launch_selected_app((void *)info);
}

static void __popup_button_add(Evas_Object *popup, struct appdata *ad)
{
	if((always_btn != NULL) && (once_btn != NULL)) {
		_D("btn is already added");
		return;
	}

	always_btn = elm_button_add(popup);

	evas_object_smart_callback_add(always_btn, "clicked", __always_btn_response_cb, ad);
	elm_object_disabled_set(always_btn, EINA_TRUE);

	once_btn = elm_button_add(popup);

	evas_object_smart_callback_add(once_btn, "clicked", __once_btn_response_cb, ad);
	elm_object_disabled_set(once_btn, EINA_TRUE);

	elm_object_style_set(always_btn, "popup");
	elm_object_style_set(once_btn, "popup");
	
	elm_object_domain_translatable_text_set(always_btn, PACKAGE, "IDS_COM_BODY_ALWAYS");	// SYSSTRING is used in Tizen 2.4
	elm_object_domain_translatable_text_set(once_btn, PACKAGE, "IDS_COM_BODY_JUST_ONCE");

	elm_object_part_content_set(popup, "button1", always_btn);
	elm_object_part_content_set(popup, "button2", once_btn);

	evas_object_show(always_btn);
	evas_object_show(once_btn);
}

void popup_button_del(Evas_Object *popup)
{
	_D("popup_button_del");

	if((always_btn == NULL) || (once_btn == NULL)) {
		_D("btn is already deleted");
		return;
	}

	elm_object_part_content_unset(popup, "button1");
	evas_object_del(always_btn);
	always_btn = NULL;

	elm_object_part_content_unset(popup, "button2");
	evas_object_del(once_btn);
	once_btn = NULL;
}

void default_popup_button_del(Evas_Object *popup)
{
	_D("default_popup_button_del");

	if(ok_btn == NULL) {
		_D("btn is already deleted");
		return;
	}

	elm_object_part_content_unset(popup, "button1");
	evas_object_del(ok_btn);
	ok_btn = NULL;
}

int popup_button_disabled_set(int disabled, struct appdata *ad)
{
	if (always_btn)
		elm_object_disabled_set(always_btn, disabled);

	if (once_btn)
		elm_object_disabled_set(once_btn, disabled);

	return 0;
}

static void __set_popup_layout(int rot, struct appdata *ad)
{
	int count;
	int width, height, screen_width;

	count = eina_list_count(ad->list);

	_D("count : %d, rot : %d", count, rot);

	width = PANEL_WIDTH;
	if (rot == 90 || rot == 270) {
		ad->item_per_row = __ITEM_ROW_CNT;
		if (count > __ITEM_ROW_CNT) { //need scroller
			height = ITEM_PADDING_HEIGHT + ITEM_HEIGHT + INDEX_HEIGHT + ITEM_PADDING_HEIGHT;
		}
		else {
			height = ITEM_PADDING_HEIGHT + ITEM_HEIGHT + ITEM_PADDING_HEIGHT;
		}
		screen_width = ad->root_h;
	}
	else {
		if (count <= __ITEM_ROW_CNT) {
			ad->item_per_row = __ITEM_ROW_CNT;
			height = ITEM_PADDING_HEIGHT + ITEM_HEIGHT + ITEM_PADDING_HEIGHT;
		}
		else if (count > __ITEM_ROW_CNT * 2) { // need scroller
			ad->item_per_row = __ITEM_ROW_CNT * 2;
			height = ITEM_PADDING_HEIGHT + ITEM_HEIGHT + ITEM_HEIGHT + INDEX_HEIGHT + ITEM_PADDING_HEIGHT;
		}
		else {
			ad->item_per_row = __ITEM_ROW_CNT * 2;
			height = ITEM_PADDING_HEIGHT + ITEM_HEIGHT + ITEM_HEIGHT + ITEM_PADDING_HEIGHT;
		}
		screen_width = ad->root_w;
	}

	ad->layout_width = ELM_SCALE_SIZE(width);
	/* Sometimes, ELM_SCALE_SIZE does not reflect real screen width correctly. */
	_D("screen width: %d layout_width: %d", screen_width, ad->layout_width);
	ad->layout_width = ad->layout_width > screen_width ? screen_width : ad->layout_width;
	ad->layout_height = ELM_SCALE_SIZE(height);

	if (ad->layout) {
		_layout_destroy(ad->layout);
	}

	ad->layout = _layout_create(ad);
}

static void __rotate_cb(void *data, Evas_Object *obj, void *event)
{
	struct appdata *ad = data;
	int angle = 0;

	angle = elm_win_rotation_get(obj);
	if (angle < 0) {
		angle += 360;
	}
	_E("angle: %d", angle);

	__set_popup_layout(angle, ad);
}

int popup_load_app_list(struct appdata *ad)
{
	Evas_Object *popup;
	int rot = -1;

	elm_app_base_scale_set(2.6);

	popup = elm_popup_add(ad->win);
	ad->popup = popup;
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_domain_translatable_part_text_set(
			popup, "title,text", PACKAGE,
			"IDS_COM_HEADER_SELECT_APPLICATION");

	rot = elm_win_rotation_get(ad->win);

	__set_popup_layout(rot, ad);

	if (ad->list && !ad->extra) {
		__popup_button_add(popup, ad);
	}

	evas_object_smart_callback_add(ad->win, "wm,rotation,changed", __rotate_cb, ad);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, __popup_back_cb, ad);

	evas_object_show(popup);

	return 0;
}
