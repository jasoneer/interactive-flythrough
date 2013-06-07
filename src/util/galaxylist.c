
#include "galaxylist.h"

#include <stdlib.h>
#include <string.h>

struct galaxylist
{
	int size;
	int index;
	GALAXY *array;
};

GALAXYLIST galaxylist_create(void)
{
	GALAXYLIST list = (GALAXYLIST)malloc(sizeof(struct galaxylist));
	if(list == NULL) return NULL;
	list->size = 1;
	list->index = 0;
	list->array = (GALAXY *)calloc(list->size, sizeof(GALAXY));
	if(list->array == NULL) {
		free(list);
		return NULL;
	}
	return list;
}
void galaxylist_delete(GALAXYLIST list)
{
	if(list == NULL) return;
	free(list->array);
	free(list);
}
void galaxylist_destroy(GALAXYLIST list)
{
	int i;
	if(list == NULL) return;
	for(i = 0; i < list->index; ++i) {
		galaxy_destroy(list->array[i]);
	}
	free(list->array);
	free(list);
}

int galaxylist_size(GALAXYLIST list)
{
	if(list == NULL) return -1;
	return list->index;
}
GALAXY *galaxylist_array(GALAXYLIST list)
{
	if(list == NULL) return NULL;
	return list->array;
}

void galaxylist_reset(GALAXYLIST list)
{
	if(list == NULL) return;
	list->index = 0;
}

static void listresize(GALAXYLIST list, int size)
{
	GALAXY *array = (GALAXY *)calloc(size, sizeof(GALAXY));
	if(array == NULL) return;
	memcpy(array, list->array, list->index * sizeof(GALAXY));
	free(list->array);
	list->array = array;
	list->size = size;
}

void galaxylist_compact(GALAXYLIST list)
{
	if(list == NULL) return;
	if(list->index >= list->size) return;
	listresize(list, list->index);
}

void galaxylist_add(GALAXYLIST list, GALAXY galaxy)
{
	if(list == NULL) return;
	if(list->index == list->size)
		listresize(list, list->index * 2);

	list->array[list->index++] = galaxy;
}