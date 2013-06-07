
#include "galaxy.h"

#include <SDL/SDL_opengl.h>
#include <stdlib.h>

#include <math.h>
#include "maths.h"

struct galaxy
{
	double x;
	double y;
	double z;
	double s;
	int t;
	int f;
	double d;
};

GALAXY galaxy_create(double x, double y, double z, double s, int t, int f)
{
	GALAXY galaxy = (GALAXY)malloc(sizeof(struct galaxy));
	if(galaxy == NULL) return NULL;
	galaxy->x = x;
	galaxy->y = y;
	galaxy->z = z;
	galaxy->s = s;
	galaxy->t = t;
	galaxy->f = f;

	galaxy->d = sqrt(x*x + y*y + z*z);
	return galaxy;
}
void galaxy_destroy(GALAXY galaxy)
{
	if(galaxy == NULL) return;
	free(galaxy);
}

double galaxy_x(GALAXY galaxy)
{
	if(galaxy == NULL) return 0.0;
	return galaxy->x;
}
double galaxy_y(GALAXY galaxy)
{
	if(galaxy == NULL) return 0.0;
	return galaxy->y;
}
double galaxy_z(GALAXY galaxy)
{
	if(galaxy == NULL) return 0.0;
	return galaxy->z;
}
double galaxy_s(GALAXY galaxy)
{
	if(galaxy == NULL) return 0.0;
	return galaxy->s;
}
int galaxy_t(GALAXY galaxy)
{
	if(galaxy == NULL) return -1;
	return galaxy->t;
}
double galaxy_d(GALAXY galaxy)
{
	if(galaxy == NULL) return 0.0;
	return galaxy->d;
}

void galaxy_update(GALAXY galaxy, double cx, double cy, double cz)
{
	if(galaxy == NULL) return;

	double x = galaxy->x - cx;
	double y = galaxy->y - cy;
	double z = galaxy->z - cz;
	galaxy->d = sqrt(x*x + y*y + z*z);
}

int galaxy_compare(const void *lhs, const void *rhs)
{
	GALAXY *g1, *g2;
	g1 = (GALAXY *)lhs;
	g2 = (GALAXY *)rhs;
	return (int)((*g1)->d - (*g2)->d);
}

static void billboardSphericalBegin(
			float camX, float camY, float camZ,
			float objPosX, float objPosY, float objPosZ) {

	float lookAt[3],objToCamProj[3], objToCam[3], upAux[3];
	float angleCosine;

	glPushMatrix();

// objToCamProj is the vector in world coordinates from the 
// local origin to the camera projected in the XZ plane
	objToCamProj[0] = camX - objPosX ;
	objToCamProj[1] = 0;
	objToCamProj[2] = camZ - objPosZ ;

// This is the original lookAt vector for the object 
// in world coordinates
	lookAt[0] = 0;
	lookAt[1] = 0;
	lookAt[2] = 1;


// normalize both vectors to get the cosine directly afterwards
	mathsNormalize(objToCamProj);

// easy fix to determine wether the angle is negative or positive
// for positive angles upAux will be a vector pointing in the 
// positive y direction, otherwise upAux will point downwards
// effectively reversing the rotation.

	mathsCrossProduct(upAux,lookAt,objToCamProj);

// compute the angle
	angleCosine = mathsInnerProduct(lookAt,objToCamProj);

// perform the rotation. The if statement is used for stability reasons
// if the lookAt and objToCamProj vectors are too close together then 
// |angleCosine| could be bigger than 1 due to lack of precision
   if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
      glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[1], upAux[2]);	
      
// so far it is just like the cylindrical billboard. The code for the 
// second rotation comes now
// The second part tilts the object so that it faces the camera

// objToCam is the vector in world coordinates from 
// the local origin to the camera
	objToCam[0] = camX - objPosX;
	objToCam[1] = camY - objPosY;
	objToCam[2] = camZ - objPosZ;

// Normalize to get the cosine afterwards
	mathsNormalize(objToCam);

// Compute the angle between objToCamProj and objToCam, 
//i.e. compute the required angle for the lookup vector

	angleCosine = mathsInnerProduct(objToCamProj,objToCam);


// Tilt the object. The test is done to prevent instability 
// when objToCam and objToCamProj have a very small
// angle between them

	if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
		if (objToCam[1] < 0)
			glRotatef(acos(angleCosine)*180/3.14,1,0,0);	
		else
			glRotatef(acos(angleCosine)*180/3.14,-1,0,0);	
      }
}

//static unsigned int openglIndex;

/*
void galaxies_init(void)
{
	openglIndex = glGenLists(1);
	if(openglIndex) {
		glNewList(openglIndex, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2d(-0.5, -0.5);

				glTexCoord2i(0, 1);
				glVertex2d(+0.5, -0.5);

				glTexCoord2i(1, 1);
				glVertex2d(+0.5, +0.5);

				glTexCoord2i(1, 0);
				glVertex2d(-0.5, +0.5);
			glEnd();
		glEndList();
	}
}
*/

void galaxy_draw(GALAXY galaxy, unsigned int *typeTextures, double cx, double cy, double cz, short useFake)
{
	if(galaxy == NULL) return;

	double size = (galaxy->s > 2) ? 2 : galaxy->s;
	if(useFake) {
		glBindTexture(GL_TEXTURE_2D, typeTextures[galaxy->f]);
		size *= 0.5;
	}
	else glBindTexture(GL_TEXTURE_2D, typeTextures[galaxy->t]);
//	if(openglIndex) {
		glPushMatrix();
			glTranslated(galaxy->x, galaxy->y, galaxy->z);
			billboardSphericalBegin(cx, cy, cz, galaxy->x, galaxy->y, galaxy->z);
			glScaled(size, size, size);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2d(-0.5, -0.5);

				glTexCoord2i(0, 1);
				glVertex2d(+0.5, -0.5);

				glTexCoord2i(1, 1);
				glVertex2d(+0.5, +0.5);

				glTexCoord2i(1, 0);
				glVertex2d(-0.5, +0.5);
			glEnd();
		glPopMatrix();
//	}
}

