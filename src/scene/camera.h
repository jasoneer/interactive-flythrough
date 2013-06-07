#pragma once

#include "../input.h"

typedef struct _plane
{
	float a;
	float b;
	float c;
	float d;
} plane;

typedef struct camera *CAMERA;

CAMERA camera_create(void);
void camera_destroy(CAMERA camera);

void camera_reset(CAMERA camera);
void camera_transform(CAMERA camera, input userinput);
void camera_frustum(CAMERA camera, double *matrix);
plane *camera_planes(CAMERA camera);

double camera_angleX(CAMERA camera);
double camera_angleY(CAMERA camera);

double camera_positionX(CAMERA camera);
double camera_positionY(CAMERA camera);
double camera_positionZ(CAMERA camera);