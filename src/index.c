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

#include "log.h"
#include "index.h"

#define PRIVATE_DATA_KEY_CUR "pdkc"



void _index_destroy(Evas_Object *index)
{
	ret_if(NULL == index);

	evas_object_data_del(index, PRIVATE_DATA_KEY_CUR);
	elm_index_item_clear(index);
	evas_object_del(index);
}



Evas_Object *_index_create(Evas_Object *layout, unsigned int count)
{
	Evas_Object *index = NULL;
	register size_t i = 0;

	retv_if(NULL == layout, NULL);

	index = elm_index_add(layout);
	retv_if(NULL == index, NULL);
	elm_object_style_set(index, "pagecontrol");

	elm_index_horizontal_set(index, EINA_TRUE);
	elm_index_autohide_disabled_set(index, EINA_TRUE);

	for (; i < count; i++) {
		elm_index_item_append(index, NULL, NULL, (void *) i);
	}
	elm_index_level_go(index, 0);
	evas_object_data_set(index, PRIVATE_DATA_KEY_CUR, NULL);
	evas_object_show(index);

	return index;
}



void _index_update(Evas_Object *index, unsigned int count)
{
	register size_t i = 0;
	size_t cur_index = 0;

	elm_index_item_clear(index);

	for (; i < count; i++) {
		elm_index_item_append(index, NULL, NULL, (void *) i);
	}
	elm_index_level_go(index, 0);
	evas_object_show(index);

	cur_index = (size_t) evas_object_data_get(index, PRIVATE_DATA_KEY_CUR);
	_index_bring_in(index, cur_index);
}



void _index_bring_in(Evas_Object *index, size_t idx)
{
	Elm_Object_Item *idx_it;

	idx_it = elm_index_item_find(index, (void *) idx);
	ret_if(NULL == idx_it);
	elm_index_item_selected_set(idx_it, EINA_TRUE);

	evas_object_data_set(index, PRIVATE_DATA_KEY_CUR, (void *) idx);
}



// End of the file
