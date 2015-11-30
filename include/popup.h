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


#ifndef __TIZEN_APP_SELECTOR_POPUP_H__
#define __TIZEN_APP_SELECTOR_POPUP_H__

#include "type.h"

int popup_load_info(struct appdata *ad, const char *msg);
int popup_load_app_list(struct appdata *ad);
int popup_button_disabled_set(int disabled, struct appdata *ad);
void popup_button_del(Evas_Object *popup);
void default_popup_button_del(Evas_Object *popup);
#endif
