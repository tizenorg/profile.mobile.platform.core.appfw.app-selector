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

#include <stdio.h>
#include <glib.h>
#include <bundle.h>
#include <bundle_internal.h>
#include <aul.h>
#include <aul_svc.h>
#include <app_control.h>
#include <app_control_internal.h>
#include <aul_svc.h>
#include <appsvc.h>
struct ucred;

#include "log.h"
#include "util.h"
#include "popup.h"
#include "layout.h"

#define MAX_MIME_STR_SIZE 256

static int __set_default_app(const char *appid, const char *operation,
		const char *mime, const char *uri)
{
	int ret;
	app_control_h app_control;

	if (appid == NULL || operation == NULL) {
		_E("Invalid parameter");
		return -1;
	}

	ret = app_control_create(&app_control);
	if (ret != APP_CONTROL_ERROR_NONE) {
		_E("Failed to create app_control handle");
		return ret;
	}

	ret = app_control_set_operation(app_control, operation);
	if (ret != APP_CONTROL_ERROR_NONE) {
		_E("Failed to set operation: %s", operation);
		goto end;
	}

	if (uri) {
		ret = app_control_set_uri(app_control, uri);
		if (ret != APP_CONTROL_ERROR_NONE) {
			_E("Failed to set uri: %s", uri);
			goto end;
		}
	}

	if (mime) {
		ret = app_control_set_mime(app_control, mime);
		if (ret != APP_CONTROL_ERROR_NONE) {
			_E("Failed to set mime: %s", mime);
			goto end;
		}
	}

	ret = app_control_set_defapp(app_control, appid);
	if (ret != APP_CONTROL_ERROR_NONE)
		_E("Failed to set default app: %s", appid);

end:
	app_control_destroy(app_control);

	return ret;
}

void _util_set_as_default(void *data)
{
	int ret;
	const char *val;
	GRegex *regex = NULL;
	GMatchInfo *match_info = NULL;
	GError *error = NULL;
	char *scheme = NULL;
	item_s *info;

	if (!data)
		return;

	info = (item_s *)data;

	_D("set as default");

	if (info->ad->control_op) {
		if (info->ad->control_uri) {
			if (strncmp(info->ad->control_uri, "/", 1) == 0){
				if (!info->ad->control_mime) {
					info->ad->control_mime = malloc(MAX_MIME_STR_SIZE);
					aul_get_mime_from_file(info->ad->control_uri, info->ad->control_mime, MAX_MIME_STR_SIZE);
				}
				info->ad->control_uri = NULL;
			} else if (strncmp(info->ad->control_uri, "file:///", 8) == 0){
				if (!info->ad->control_mime) {
					info->ad->control_mime = malloc(MAX_MIME_STR_SIZE);
					aul_get_mime_from_file(&info->ad->control_uri[7], info->ad->control_mime, MAX_MIME_STR_SIZE);
				}
				info->ad->control_uri = NULL;
			} else if (strncmp(info->ad->control_uri, "file:/", 6)==0){
				if (!info->ad->control_mime) {
					info->ad->control_mime = malloc(MAX_MIME_STR_SIZE);
					aul_get_mime_from_file(&info->ad->control_uri[5], info->ad->control_mime, MAX_MIME_STR_SIZE);
				}
				info->ad->control_uri = NULL;
			}
		}

		val = bundle_get_val(info->ad->kb, AUL_SVC_K_URI_R_INFO);
		if (val == NULL) {
			if (info->ad->control_uri) {
				regex = g_regex_new("^(([^:/?#]+):)?", 0, 0, &error);
				if (regex == NULL)
					_E("Failed to create GRegex");

				if (g_regex_match(regex, info->ad->control_uri, 0, &match_info) == FALSE) {
					_D("reg match fail for set_as_default");
					g_regex_unref(regex);
					return;
				}

				scheme = g_match_info_fetch(match_info, 2);
			}

			ret = __set_default_app(info->appid, info->ad->control_op, info->ad->control_mime, scheme);

			if (scheme)
				g_free(scheme);
			if (match_info)
				g_match_info_free(match_info);
			if (regex)
				g_regex_unref(regex);
		} else {
			ret = __set_default_app(info->appid, info->ad->control_op, info->ad->control_mime, val);
			bundle_del(info->ad->kb, AUL_SVC_K_URI_R_INFO);
		}

		if (ret != 0)
			_E("Failed to set default app: %d", ret);
	}

	_D("done");
}

void _util_cancel(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	bundle *kb;
	int ret;
	const char *pid = NULL;

	pid = bundle_get_val(ad->kb, AUL_K_CALLER_PID);
	if(pid == NULL) {
		_E("get CALLER PID ERROR");
		return;
	} else {
		_D("CALLER PID(%s)", pid);
	}

	kb = bundle_create();
	if(kb == NULL) {
		_E("bundle create error");
		return;
	}
	bundle_add(kb, AUL_K_SEND_RESULT, "1");
	bundle_add(kb, AUL_K_CALLER_PID, pid);

	ret = aul_svc_send_result(kb, AUL_SVC_RES_CANCEL);

	if (ret != AUL_SVC_RET_OK) {
		_E("aul_send_result error(%d)", ret);
	}

	bundle_free(kb);
}

void _util_recycle_app(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	const char *val_launch = NULL;

	ad->lower_on_pause = 0;

	if (ad->list) {
		eina_list_free(ad->list);
		ad->list = NULL;
	}

	if (ad->popup) {
		popup_button_del(ad->popup);
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->default_popup) {
		default_popup_button_del(ad->default_popup);
		evas_object_del(ad->default_popup);
		ad->default_popup = NULL;
	}

	if (ad->layout) {
		_layout_destroy(ad->layout);
		ad->layout = NULL;
	}

	if (ad->kb) {
		val_launch = bundle_get_val(ad->kb, "__APP_SVC_START_INFO__");

		if (!val_launch) {
			_D("no app selected or launched. send cancel to caller");
			_util_cancel(ad);
		}

		bundle_free(ad->kb);
		ad->kb = NULL;
	}

	ad->selected = NULL;
	ad->selected_index = -1;

	int exit;
	aul_app_group_lower(&exit);
	elm_win_lower(ad->win);
}

static Eina_Bool __lower_timeout(void *data)
{
	struct appdata *ad = (struct appdata *)data;

	_D("lower timeout expired");
	ad->lower_timer = NULL;
	if (ad->lower_on_pause == 1)
		_util_recycle_app(ad);

	return ECORE_CALLBACK_CANCEL;
}

#define LOWER_TIMEOUT 5.0
void _util_launch_selected_app(void *data)
{
	int ret = 0;

	if (!data)
		return;

	item_s *info = (item_s *)data;

	bundle_add(info->ad->kb, AUL_SVC_K_RUA_STAT_CALLER, "app-selector");
	bundle_add(info->ad->kb, AUL_SVC_K_RUA_STAT_TAG, info->appid);
	ret = aul_forward_app(info->appid, info->ad->kb);
	if(ret < 0) {
		_SE("app(%s) launch error", info->appid);
	} else {
		_SD("app(%s) launch ok", info->appid);
		bundle_add(info->ad->kb, APP_SVC_START_INFO, info->appid);
	}

	info->ad->lower_on_pause = 1;
	info->ad->lower_timer = ecore_timer_add(LOWER_TIMEOUT,
					__lower_timeout,
					info->ad);
	if (info->ad->lower_timer == NULL) {
		_SE("failed to set timer for lowering");
	}
}

