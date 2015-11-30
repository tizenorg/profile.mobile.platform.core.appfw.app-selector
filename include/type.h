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

#ifndef __TIZEN_APP_SELECTOR_TYPE_H__
#define __TIZEN_APP_SELECTOR_TYPE_H__

#include <Elementary.h>
#include <app.h>
#include <bundle.h>

#define APP_SVC_START_INFO "__APP_SVC_START_INFO__"


struct _item {
	struct appdata *ad;
	int idx;
	int extra;
	bool is_preloaded;
	int score;
        char *appid;
        char *name;
        char *icon;
	Elm_Object_Item *grid_item;
};
typedef struct _item item_s;


struct appdata {
	Evas_Object *win;
	Evas_Object *popup;
	Evas_Object *layout;
	Evas_Object *layout_rect;
	Evas_Object *default_popup;
	Evas_Object *index;
	Evas_Object *scroller;
	item_s *selected;
	int rua_stat_score;
	int selected_index;
	int extra;
	int layout_width;
	int layout_height;
	int root_w;
	int root_h;
	int item_per_row;
	int lower_on_pause;
	Ecore_Timer *lower_timer;
	Eina_List *list;
	bundle *kb;
	char *control_op;
	char *control_mime;
	char *control_uri;
};

#endif
