#pragma once

#include "../scene/galaxy.h"

typedef struct galaxylist *GALAXYLIST;

GALAXYLIST galaxylist_create(void);
void galaxylist_delete(GALAXYLIST list);
void galaxylist_destroy(GALAXYLIST list);

void galaxylist_reset(GALAXYLIST list);
void galaxylist_compact(GALAXYLIST list);

int galaxylist_size(GALAXYLIST list);
GALAXY *galaxylist_array(GALAXYLIST list);

void galaxylist_add(GALAXYLIST list, GALAXY galaxy);