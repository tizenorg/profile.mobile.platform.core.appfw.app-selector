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

#ifndef __TIZEN_APP_SELECTOR_LOG_H__
#define __TIZEN_APP_SELECTOR_LOG_H__

#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "APP-SELECTOR"

#if !defined(_D)
#define _D(fmt, arg...) LOGD(fmt"\n", ##arg)
#endif

#if !defined(_W)
#define _W(fmt, arg...) LOGW(fmt"\n", ##arg)
#endif

#if !defined(_E)
#define _E(fmt, arg...) LOGE(fmt"\n", ##arg)
#endif

#if !defined(_SD)
#define _SD(fmt, arg...) SECURE_LOGD(fmt"\n", ##arg)
#endif

#if !defined(_SW)
#define _SW(fmt, arg...) SECURE_LOGW(fmt"\n", ##arg)
#endif

#if !defined(_SE)
#define _SE(fmt, arg...) SECURE_LOGE(fmt"\n", ##arg)
#endif

#define retv_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#define ret_if(expr) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)

#define goto_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> goto", #expr); \
		goto val; \
	} \
} while (0)

#define break_if(expr) { \
	if(expr) { \
		_E("(%s) -> break", #expr); \
		break; \
	} \
}

#define continue_if(expr) { \
	if(expr) { \
		_E("(%s) -> continue", #expr); \
		continue; \
	} \
}

#endif /* __TIZEN_APP_SELECTOR_LOG_H__ */
