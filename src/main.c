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

#include <app.h>
#include <string.h>
#include <Elementary.h>
#include <pkgmgr-info.h>
#include <bundle.h>
#include <bundle_internal.h>
#include <Evas.h>
#include <appsvc.h>
#include <rua_stat.h>

#include "type.h"
#include "log.h"
#include "popup.h"
#include "util.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__ ((visibility("default")))
#endif

extern int app_control_export_as_bundle(app_control_h app_control, bundle **data);

static void __win_del(void *data, Evas_Object * obj, void *event)
{
	ui_app_exit();
}

static Evas_Object *__create_win(const char *name, struct appdata *ad)
{
	Evas_Object *eo;
	Evas *e;
	Ecore_Evas *ee;

	if (ad == NULL) {
		_E("invalid argument");
		return NULL;
	}

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		elm_win_alpha_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request",
					       __win_del, NULL);
		elm_win_screen_size_get(eo, NULL, NULL, &(ad->root_w), &(ad->root_h));
		evas_object_resize(eo, ad->root_w, ad->root_h);

		if (elm_win_wm_rotation_supported_get(eo)) {
			int rots[4] = { 0, 90, 180, 270 };
			elm_win_wm_rotation_available_rotations_set(eo, rots, 4);
		}

		e = evas_object_evas_get(eo);
		if (!e)
			goto error;

		ee = ecore_evas_ecore_evas_get(e);
		if (!ee)
			goto error;

		ecore_evas_name_class_set(ee, "SYSTEM_POPUP", "SYSTEM_POPUP");

		evas_object_show(eo);
	}

	return eo;

error:
	if (eo)
		evas_object_del(eo);

	return NULL;
}

static bool _create(void *data)
{
	_D("on create");
	struct appdata *ad = (struct appdata *)data;
	Evas_Object *win;

	win = __create_win(PACKAGE, ad);
	if (!win) {
		_E("failed to create window");
		return false;
	}

	ad->win = win;

	return true;
}

static void _terminate(void *data)
{
	_D("on terminate");
	struct appdata *ad = (struct appdata *)data;
	const char *val_launch = NULL;

	if (!ad)
		return;

	if (ad->kb) {
		val_launch = bundle_get_val(ad->kb, "__APP_SVC_START_INFO__");

		if (!val_launch)
			_util_cancel(ad);

		bundle_free(ad->kb);
		ad->kb = NULL;
	}

	if (ad->control_op) {
		free(ad->control_op);
		ad->control_op = NULL;
	}

	if (ad->control_mime) {
		free(ad->control_mime);
		ad->control_mime = NULL;
	}

	if (ad->control_uri) {
		free(ad->control_uri);
		ad->control_uri = NULL;
	}

	if (ad->list) {
		eina_list_free(ad->list);
		ad->list = NULL;
	}
}

static void _pause(void *data)
{
	struct appdata *ad = (struct appdata *)data;

	_D("on pause");
	if (ad->lower_on_pause == 1) {
		_D("lower on pause");
		ad->lower_on_pause = 0;
		if (ad->lower_timer)
			ecore_timer_del(ad->lower_timer);
		ad->lower_timer = NULL;
		_util_recycle_app(ad);
	}
}

static void _resume(void *resume)
{
	_D("on resume");
}

static bool __iterate(app_control_h app_control, const char *app_id, void *data)
{
	struct appdata *ad = (struct appdata *)data;
	item_s *info;
	int ret = 0;
	pkgmgrinfo_appinfo_h handle = NULL;
	char *str = NULL;
	bool is_preloaded = false;

	ret = pkgmgrinfo_appinfo_get_usr_appinfo(app_id, getuid(), &handle);
	if (ret != PMINFO_R_OK) {
		return -1;
	}

	info = calloc(1, sizeof(item_s));
	if (!info) {
		_E("out of memory");
		return false;
	}

	info->ad = ad;

	ret = pkgmgrinfo_appinfo_is_preload(handle, &is_preloaded);
	info->is_preloaded = is_preloaded;

	ret = pkgmgrinfo_appinfo_get_label(handle, &str);
	if ((ret == PMINFO_R_OK) && (str)) {
		info->name = strdup(str);
	}
	str = NULL;

	ret = pkgmgrinfo_appinfo_get_appid(handle, &str);
	if ((ret == PMINFO_R_OK) && (str)) {
		info->appid = strdup(str);
	}
	str = NULL;

	ret = pkgmgrinfo_appinfo_get_icon(handle, &str);
	if ((ret == PMINFO_R_OK) && (str)) {
		info->icon = strdup(str);
	}
	str = NULL;

	_SD("APPID : %s, NAME : %s, ICON : %s, is_preloaded : %d\n", info->appid, info->name,
		       info->icon, info->is_preloaded);

	info->ad = data;
	info->extra = ad->extra;

	ad->list = eina_list_append(ad->list, info);


	ret = pkgmgrinfo_appinfo_destroy_appinfo(handle);
	if (ret != PMINFO_R_OK) {
		_D("destroy appinfo handle error(%d)", ret);
		return false;
	}

	return true;
}

int __rua_stat_tag_iter_fn(const char *rua_stat_tag, void *data) {

	struct appdata *ad = (struct appdata *)data;
	Eina_List *l;
	Eina_List *ln;
	item_s *app_info;

	EINA_LIST_FOREACH_SAFE(ad->list, l, ln, app_info) {
		if (strcmp(app_info->appid, rua_stat_tag) == 0) {
			app_info->score += ad->rua_stat_score;
			ad->rua_stat_score -= 10;
			break;
		}
	}
	return 0;

}

static int __rua_stat_score_cmp(const void *a, const void *b) {

	item_s *app_a = (item_s *)a;
	item_s *app_b = (item_s *)b;
	int result = 0;

	if (app_b->is_preloaded == app_a->is_preloaded) {
		result = app_b->score - app_a->score;
		if (result == 0)
			result = strcmp(app_b->appid, app_a->appid);
	} else {
		if (app_b->is_preloaded)
			result = 1;
		else
			result = -1;
	}

	return result;
}

static void _app_control(app_control_h app_control, void *data)
{
	_D("on appcontrol");
	struct appdata *ad = (struct appdata *)data;
	int ret = 0;
	int i = 0;
	const char **extra_list = NULL;
	int extra_list_cnt = 0;
	const char *str = NULL;
	Eina_List *l;
	item_s *li;
	int lcnt = 0;

	if (!ad)
		return;

	ad->selected_index = -1;
	ad->selected = NULL;

	app_control_export_as_bundle(app_control, &(ad->kb));

	app_control_get_operation(app_control, &ad->control_op);
	app_control_get_mime(app_control, &ad->control_mime);
	app_control_get_uri(app_control, &ad->control_uri);

	ret = bundle_get_type(ad->kb, APP_SVC_K_SELECTOR_EXTRA_LIST);
	if(ret != BUNDLE_TYPE_NONE) {
		_E("has extra list. launch without buttons");
		ad->extra = 1;
		if(ret & BUNDLE_TYPE_ARRAY) {
			extra_list = bundle_get_str_array(ad->kb, APP_SVC_K_SELECTOR_EXTRA_LIST, &extra_list_cnt);
			_D("extra list cnt : %d", extra_list_cnt);
		} else {
			str = bundle_get_val(ad->kb, APP_SVC_K_SELECTOR_EXTRA_LIST);
			extra_list = (const char **)(&str);
			extra_list_cnt = 1;
		}
	}
	else
		ad->extra = 0;

	for (i = 0; i < extra_list_cnt; i++) {
		if (false == __iterate(app_control, extra_list[i], ad))
			break;
	}

	app_control_foreach_app_matched(app_control, __iterate, ad);

	if (!ad->control_op || ad->list == NULL)
		popup_load_info(ad, _("IDS_COM_BODY_NO_APPLICATIONS_CAN_PERFORM_THIS_ACTION"));
	else {
		int list_length = eina_list_count(ad->list);

		if (list_length == 1) {
			item_s *one = eina_list_data_get(ad->list);
			if (one) {
				_util_launch_selected_app(one);
				ui_app_exit();
			}

			return;
		}

		ad->rua_stat_score = (list_length + 1) * 10;
		rua_stat_get_stat_tags("app-selector", __rua_stat_tag_iter_fn, ad);

		ad->list = eina_list_sort(ad->list, list_length, __rua_stat_score_cmp);
		EINA_LIST_FOREACH(ad->list, l, li) {
			if (li) {
				li->idx = lcnt;
				lcnt++;
			}
		}

		popup_load_app_list(ad);
	}
}

static void __orientation_changed_cb(app_event_info_h event_info, void *user_data)
{
	app_device_orientation_e e;
	app_event_get_device_orientation(event_info, &e);
	_D("orientation changed :%d", e);
}

static void __language_changed_cb(app_event_info_h event_info, void *user_data)
{
	char *lang = NULL;
	int ret;

	_D("language changed");

	ret = app_event_get_language(event_info, &lang);
	if (ret != APP_ERROR_NONE) {
		_E("Fail to get string value(%d)", ret);
		return;
	}

	_D("lang: %s", lang);

	if (!lang)
		return;

	elm_language_set(lang);

	free(lang);
}

EXPORT_API int main(int argc, char *argv[])
{
	int ret = 0;

	struct appdata ad;
	app_event_handler_h orientation_h;
	app_event_handler_h language_h;


	memset(&ad, 0, sizeof(struct appdata));

	ui_app_lifecycle_callback_s lifecycle_callback = {0,};

	lifecycle_callback.create = _create;
	lifecycle_callback.terminate = _terminate;
	lifecycle_callback.pause = _pause;
	lifecycle_callback.resume = _resume;
	lifecycle_callback.app_control = _app_control;
	ui_app_add_event_handler(&orientation_h, APP_EVENT_DEVICE_ORIENTATION_CHANGED, __orientation_changed_cb, &ad);
	ui_app_add_event_handler(&language_h, APP_EVENT_LANGUAGE_CHANGED, __language_changed_cb, &ad);

	ret = ui_app_main(argc, argv, &lifecycle_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		_E("app_main() is failed. err = %d", ret);
	}

	return ret;
}
