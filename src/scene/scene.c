
#include "scene.h"
#include "camera.h"

#include "../input.h"
#include "../util/bitmap.h"
#include "../util/octree.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

//#if defined(__MACOSX__)
//#include <OpenGL/glu.h>
//#else
//#include <GL/glu.h>
//#endif

#include <stdlib.h>
#include <stdio.h>

struct _scene
{
	CAMERA camera;
	OCTNODE octree;
	GALAXYLIST galaxies;
	short realImages;

	BITMAP typeImages[NUM_TYPES];
	GLuint typeTextures[NUM_TYPES];
};
static struct _scene scene;

static void load_bitmaps(void)
{
	int i;
	for(i = 22; i < NUM_TYPES; ++i)
	{
		char num[5], *path, *part, *full;
		sprintf(num, "%04d", i);

		path = input_path_concat("types/gal_");
		part = concat(path, num);
		full = concat(part, ".bmp");

		scene.typeImages[i] = bitmap_read(full);
		free(path); free(part); free(full);
	}
}

void scene_load(void)
{
	load_bitmaps();
	scene.realImages = 0;
	scene.camera = camera_create();
	scene.octree = octree_init(input_path_concat("galaxies.txt"), 20000);
	scene.galaxies = galaxylist_create();
}

void scene_toggle(void)
{
	scene.realImages = !scene.realImages;
}
void scene_reset(void)
{
	camera_reset(scene.camera);
}

static void init_textures(void)
{
	int i;
	glGenTextures(NUM_TYPES, &scene.typeTextures[0]);
	for(i = 22; i < NUM_TYPES; ++i)
	{
		if(scene.typeImages[i] != NULL)
		{
			int glFormat = GL_RGB;
			if(bitmap_bytesPerPixel(scene.typeImages[i]) == 4)
				glFormat = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, scene.typeTextures[i]);
			gluBuild2DMipmaps(
				GL_TEXTURE_2D,            // the current 2D texture
				glFormat,                  //internal format for texture
				bitmap_width(scene.typeImages[i]),  // size in s coor
				bitmap_height(scene.typeImages[i]), // size in t coord
				glFormat,                  // incoming data format; should match internal
				GL_UNSIGNED_BYTE,         // type of incoming data
				bitmap_pixels(scene.typeImages[i])  // pointer to the data
			);

			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, glFormat,
						bitmap_width(scene.typeImages[i]), bitmap_height(scene.typeImages[i]),
						0, glFormat, GL_UNSIGNED_BYTE, bitmap_pixels(scene.typeImages[i]));	
		}
	}
}

void scene_init(void)
{
//	galaxies_init();

//	glClearDepth(0.0f);
//	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	init_textures();

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void scene_resize(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, width/height, 1, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void scene_update(float delta)
{
	camera_transform(scene.camera, input_get());
	octree_galaxies(scene.octree, scene.camera, scene.galaxies);
	int i, size = galaxylist_size(scene.galaxies);
	GALAXY *array = galaxylist_array(scene.galaxies);
	for(i = 0; i < size; ++i) {
		galaxy_update(array[i], camera_positionX(scene.camera),
								camera_positionY(scene.camera),
								camera_positionZ(scene.camera));
	}
	qsort(array, size, sizeof(GALAXY), galaxy_compare);
}

static void scene_transform()
{
	double matrix[16];
//	double projection[16];

	glLoadIdentity();
	glRotatef(-camera_angleX(scene.camera), 1.0f, 0.0f, 0.0f);
	glRotatef(-camera_angleY(scene.camera), 0.0f, 1.0f, 0.0f);
	glTranslatef(-camera_positionX(scene.camera),
				 -camera_positionY(scene.camera),
				 -camera_positionZ(scene.camera));

//	glGetDoublev(GL_PROJECTION_MATRIX, projection);
//	glPushMatrix();
//		glLoadIdentity();
//		glRotatef(camera_angleX(scene.camera), 1.0f, 0.0f, 0.0f);
//		glRotatef(camera_angleY(scene.camera), 0.0f, 1.0f, 0.0f);
//
//		glMultMatrixd(projection);
//		glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
//	glPopMatrix();

	camera_frustum(scene.camera, matrix);
}

void scene_render(float delta)
{
	scene_transform();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int i, size = galaxylist_size(scene.galaxies);
	GALAXY *array = galaxylist_array(scene.galaxies);
	for(i = size - 1; i >= 0; --i) {
		galaxy_draw(array[i], scene.typeTextures, camera_positionX(scene.camera),
												  camera_positionY(scene.camera),
												  camera_positionZ(scene.camera), scene.realImages);
	}
}
