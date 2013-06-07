#pragma once

#include "galaxylist.h"
#include "../scene/scene.h"
#include "../scene/camera.h"

typedef struct octnode *OCTNODE;

OCTNODE octree_init(const char *filename, int maxitems);

void octree_galaxies(OCTNODE octnode, CAMERA camera, GALAXYLIST galaxylist);