#pragma once

typedef struct galaxy *GALAXY;

GALAXY galaxy_create(double x, double y, double z, double s, int t, int f);
void galaxy_destroy(GALAXY galaxy);

double galaxy_x(GALAXY galaxy);
double galaxy_y(GALAXY galaxy);
double galaxy_z(GALAXY galaxy);
double galaxy_s(GALAXY galaxy);
int galaxy_t(GALAXY galaxy);
double galaxy_d(GALAXY galaxy);

int galaxy_compare(const void *lhs, const void *rhs);

//void galaxies_init(void);

void galaxy_update(GALAXY galaxy, double cx, double cy, double cz);
void galaxy_draw(GALAXY galaxy, unsigned int *typeTextures, double cx, double cy, double cz, short realImage);