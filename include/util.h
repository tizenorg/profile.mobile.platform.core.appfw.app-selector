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

#ifndef __TIZEN_APP_SELECTOR_UTIL_H__
#define __TIZEN_APP_SELECTOR_UTIL_H__

#include "type.h"

void _util_set_as_default(void *data);
void _util_launch_selected_app(void *data);
void _util_cancel(void *data);
void _util_recycle_app(void *data);

#endif /* __TIZEN_APP_SELECTOR_UTIL_H__ */
