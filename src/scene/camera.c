
#include "camera.h"

#include <math.h>
#include <stdlib.h>

struct camera
{
	double px;
	double py;
	double pz;

	double ax;
	double ay;

	plane pn[6];
};

CAMERA camera_create(void)
{
	CAMERA camera = (CAMERA)malloc(sizeof(struct camera));
	if(camera == NULL) return NULL;

	camera->px = 0.0f;
	camera->py = 0.0f;
	camera->pz = 0.0f;
	camera->ax = 0.0f;
	camera->ay = 0.0f;

	return camera;
}
void camera_destroy(CAMERA camera)
{
	if(camera == NULL) return;
	free(camera);
}

void camera_reset(CAMERA camera)
{
	if(camera == NULL) return;

	camera->px = 0.0f;
	camera->py = 0.0f;
	camera->pz = 0.0f;
	camera->ax = 0.0f;
	camera->ay = 0.0f;
}
void camera_transform(CAMERA camera, input userinput)
{
	if(camera == NULL) return;

	camera->ay += userinput.angle_y;
	camera->ax += userinput.angle_x;
	camera->ax = camera->ax > M_PI/2 ? M_PI/2 :
				 camera->ax < -M_PI/2 ? -M_PI/2 :
				 camera->ax;
	
	camera->py += userinput.velocity * -sin(camera->ax);
	camera->px += userinput.velocity * cos(camera->ax) * sin(camera->ay);
	camera->pz += userinput.velocity * cos(camera->ax) * cos(camera->ay);
}

static plane extract_plane(double *mat, int row)
{
	plane pln;
	int sign = (row < 0) ? -1 : 1;
	row = abs(row) - 1;

	pln.a = mat[3]  + sign * mat[row];
	pln.b = mat[7]  + sign * mat[row + 4];
	pln.c = mat[11] + sign * mat[row + 8];
	pln.d = mat[15] + sign * mat[row + 12];

	float length = sqrtf(pln.a * pln.a +
						 pln.b * pln.b +
						 pln.c * pln.c);
	pln.a /= length;
	pln.b /= length;
	pln.c /= length;
	pln.d /= length;
	return pln;
}
void camera_frustum(CAMERA camera, double *matrix)
{
	if(camera == NULL) return;

	camera->pn[0] = extract_plane(matrix,  1);
	camera->pn[1] = extract_plane(matrix, -1);
	camera->pn[2] = extract_plane(matrix,  2);
	camera->pn[3] = extract_plane(matrix, -2);
	camera->pn[4] = extract_plane(matrix,  3);
	camera->pn[5] = extract_plane(matrix, -3);
}
plane *camera_planes(CAMERA camera)
{
	if(camera == NULL) return NULL;
	return camera->pn;
}

double camera_angleX(CAMERA camera)
{
	if(camera == NULL) return 0.0;
	return camera->ax * 180 / M_PI;
}
double camera_angleY(CAMERA camera)
{
	if(camera == NULL) return 0.0;
	return camera->ay * 180 / M_PI;
}

double camera_positionX(CAMERA camera)
{
	if(camera == NULL) return 0.0;
	return camera->px;
}
double camera_positionY(CAMERA camera)
{
	if(camera == NULL) return 0.0;
	return camera->py;
}
double camera_positionZ(CAMERA camera)
{
	if(camera == NULL) return 0.0;
	return camera->pz;
}
