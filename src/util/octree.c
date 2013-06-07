
#include "octree.h"

#include <stdio.h>
#include <stdlib.h>

struct octnode
{
	double minx, miny, minz;
	double maxx, maxy, maxz;

	int hasChildren;
	OCTNODE child[8];

	GALAXYLIST galaxies;
};

static OCTNODE create_octnode(double minx, double miny, double minz, double maxx, double maxy, double maxz)
{
	int i;
	OCTNODE octnode;

	octnode = (OCTNODE)malloc(sizeof(struct octnode));
	if(octnode == NULL) return NULL;

	octnode->galaxies = galaxylist_create();
	if(octnode->galaxies == NULL) return NULL;

	octnode->hasChildren = 0;
	for(i = 0; i < 8; ++i)
		octnode->child[i] = NULL;

	octnode->minx = minx;
	octnode->miny = miny;
	octnode->minz = minz;
	octnode->maxx = maxx;
	octnode->maxy = maxy;
	octnode->maxz = maxz;
	return octnode;
}

static int create_children(OCTNODE octnode)
{
	double sx, sy, sz;
	double ex, ey, ez;
	double dx, dy, dz;
	double mx, my, mz;
	int i;

	sx = octnode->minx;
	sy = octnode->miny;
	sz = octnode->minz;

	ex = octnode->maxx;
	ey = octnode->maxy;
	ez = octnode->maxz;
	
	dx = ex - sx;
	dy = ey - sy;
	dz = ez - sz;

	mx = sx + dx / 2;
	my = sy + dy / 2;
	mz = sz + dz / 2;

	octnode->hasChildren = 1;
	octnode->child[0] = create_octnode(sx, sy, sz, mx, my, mz);
	octnode->child[1] = create_octnode(mx, sy, sz, ex, my, mz);
	octnode->child[2] = create_octnode(sx, my, sz, mx, ey, mz);
	octnode->child[3] = create_octnode(mx, my, sz, ex, ey, mz);
	octnode->child[4] = create_octnode(sx, sy, mz, mx, my, ez);
	octnode->child[5] = create_octnode(mx, sy, mz, ex, my, ez);
	octnode->child[6] = create_octnode(sx, my, mz, mx, ey, ez);
	octnode->child[7] = create_octnode(mx, my, mz, ex, ey, ez);

	for(i = 0; i < 8; ++i)
		if(octnode->child[i] == NULL)
			octnode->hasChildren = 0;

	return octnode->hasChildren;
}

static void insert_galaxy(OCTNODE octnode, GALAXY galaxy, int max);

static void insert_children(OCTNODE octnode, GALAXY galaxy, int max)
{
	int i;
    for(i = 0; i < 8; ++i)
	    if( (octnode->child[i])->minx <= galaxy_x(galaxy) &&
	    	(octnode->child[i])->miny <= galaxy_y(galaxy) &&
	    	(octnode->child[i])->minz <= galaxy_z(galaxy) &&
		    (octnode->child[i])->maxx >= galaxy_x(galaxy) &&
		    (octnode->child[i])->maxy >= galaxy_y(galaxy) &&
		    (octnode->child[i])->maxz >= galaxy_z(galaxy)) {
	    	insert_galaxy(octnode->child[i], galaxy, max);
	    	return;
	    }
	printf("Error: octree node children galaxy not inserted\n");
}

static void insert_galaxy(OCTNODE octnode, GALAXY galaxy, int max)
{
	if(!octnode->hasChildren) {
		int size = galaxylist_size(octnode->galaxies);
		if(size > max) {
			int i; GALAXY *galarray;

			if(!create_children(octnode)) {
				printf("Error: octree node children allocation failed\n");
				return;
			}

			insert_children(octnode, galaxy, max);
			galarray = galaxylist_array(octnode->galaxies);
			for(i = 0; i < size; ++i)
				insert_children(octnode, galarray[i], max);

			galaxylist_delete(octnode->galaxies);
			octnode->galaxies = NULL;
		} else galaxylist_add(octnode->galaxies, galaxy);
	} else insert_children(octnode, galaxy, max);
}

struct _stats {
	int galaxycount;
	int orphancount;
	int otnodecount;
};

static struct _stats verify_compact(OCTNODE octnode)
{
	struct _stats current;
	current.galaxycount = 0;
	current.orphancount = 0;
	current.otnodecount = 0;

	if(octnode == NULL) return current;
	current.otnodecount = 1;

	if(octnode->hasChildren) {
		int i; struct _stats children;
		if(octnode->galaxies != NULL) {
			current.orphancount = galaxylist_size(octnode->galaxies);
			if(current.orphancount == 0) {
				galaxylist_destroy(octnode->galaxies);
				octnode->galaxies = NULL;
			}
			printf("Error: octree node has [%d] orphaned galaxies\n", current.orphancount);
		}
		for(i = 0; i < 8; ++i) {
			children = verify_compact(octnode->child[i]);
			current.galaxycount += children.galaxycount;
			current.orphancount += children.orphancount;
			current.otnodecount += children.otnodecount;
		}
	} else {
		if(octnode->galaxies != NULL) {
			galaxylist_compact(octnode->galaxies);
			current.galaxycount = galaxylist_size(octnode->galaxies);
		}
		else printf("Error: octree leaf node has no galaxies\n");
	}
	return current;
}
/*
static void print_galaxies(FILE *pOut, GALAXYLIST galaxies)
{
	fprintf(pOut, " GALAXIES %d", galaxylist_size(galaxies));
//	GALAXY *array = galaxylist_array(galaxies);
//	for(i = 0; i < size; ++i)
//		fprintf(pOut, "GALAXY %lf %lf %lf %lf %d\n",
//			galaxy_x(array[i]), galaxy_y(array[i]), galaxy_z(array[i]),
//			galaxy_s(array[i]), galaxy_p(array[i]));
}
static void print_tree(FILE *pOut, OCTNODE octnode, int indent)
{
	int i;
	if(octnode == NULL) return;

	for(i = 0; i < indent; ++i)
		fprintf(pOut, "\t");
	fprintf(pOut, "OCTNODE %lf %lf %lf %lf %lf %lf",
		octnode->minx, octnode->miny, octnode->minz,
		octnode->maxx, octnode->maxy, octnode->maxz);
	print_galaxies(pOut, octnode->galaxies);
	fprintf(pOut, "\n");
	
	if(octnode->hasChildren)
		for(i = 0; i < 8; i++)
			print_tree(pOut, octnode->child[i], indent + 1);
}
*/

OCTNODE octree_init(const char *filename, int maxitems)
{
	OCTNODE root;
	FILE *fp;
	char line[100];
	double minx, miny, minz, maxx, maxy, maxz;
	struct _stats stats;
	int linecount = 0;
	
	fp = fopen(filename, "r");
	if(fp == NULL) {
		printf("Error: galaxies file at [%s] could not be opened\n", filename);
		return NULL;
	}
	fgets(line, 100, fp);
	sscanf(line, "%lf %lf %lf %lf %lf %lf", &minx, &miny, &minz, &maxx, &maxy, &maxz);
	root = create_octnode(minx, miny, minz, maxx, maxy, maxz);
	if(root == NULL) {
		printf("Error: root octree node allocation failed\n");
		return NULL;
	}
	int fakeIndex = 22;
	while(fgets(line, 100, fp) != NULL) {
		fakeIndex %= NUM_TYPES; //number of pictures in directory
		if(fakeIndex == 0)
			fakeIndex = 22;
		double x, y, z, s; int t;
		sscanf(line, "%lf %lf %lf %lf %d", &x, &y, &z, &s, &t);
		GALAXY galaxy = galaxy_create(x, y, z, s, t, fakeIndex);
		if(galaxy != NULL) {
        	insert_galaxy(root, galaxy, maxitems);
//			galaxylist_add(root->galaxies, galaxy);
        	++linecount;
		}
		fakeIndex++;
	}
	fclose(fp);

	
	stats = verify_compact(root);
	printf("Octree: Nodes[%d] Galaxies Scanned[%d] Added[%d] Orphaned[%d]\n",
		   stats.otnodecount, linecount, stats.galaxycount, stats.orphancount);
	printf("Octree: Min[%lf %lf %lf] Max[%lf %lf %lf]\n",
		   root->minx, root->miny, root->minz, root->maxx, root->maxy, root->maxz);

//	{
//		FILE *pOut = fopen("output.txt", "w");
//		print_tree(pOut, root, 0);
//	}
	
	return root;
}

typedef struct _point
{
	double x;
	double y;
	double z;
} point;

/*
static float dot_product(point v1, point v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
*/

/*
static void pn_vectors(point *aabb, point *vmin, point *vmax)
{
     int i;
	
	vmin->x = aabb[0].x;
	vmax->x = aabb[0].x;
	vmin->y = aabb[0].y;
	vmax->y = aabb[0].y;
	vmin->z = aabb[0].z;
	vmax->z = aabb[0].z;
	
	for(i = 1; i < 2; i++) {
		if( aabb[i].x > vmax->x )
               vmax->x = aabb[i].x;
          if( aabb[i].x < vmin->x )
               vmin->x = vec_arr[i].x;
          if( vec_arr[i].y > vmax->y )
               vmax->y = vec_arr[i].y;
          if( vec_arr[i].y < vmin->y )
               vmin->y = vec_arr[i].y;
          if( vec_arr[i].z > vmax->z )
               vmax->z = vec_arr[i].z;
          if( vec_arr[i].z < vmin->z )
               vmin->z = vec_arr[i].z;
	}
}*/

static int intersects(OCTNODE node, CAMERA cam)
{
	double x = camera_positionX(cam);
	double y = camera_positionY(cam);
	double z = camera_positionZ(cam);
	if(node->minx < x && node->maxx > x &&
	   node->miny < y && node->maxy > y &&
	   node->minz < z && node->maxz > z) {
		return 1;
	}
	return 0;
/*
	int i, j; point p[8];
	plane *n = camera_planes(cam);
	p[0].x = node->minx; p[1].x = node->maxx; p[2].x = node->maxx; p[3].x = node->minx;
	p[0].y = node->miny; p[1].y = node->miny; p[2].y = node->maxy; p[3].y = node->maxy;
	p[0].z = node->minz; p[1].z = node->minz; p[2].z = node->minz; p[3].z = node->minz;
	p[4].x = node->minx; p[5].x = node->maxx; p[6].x = node->maxx; p[7].x = node->minx;
	p[4].y = node->miny; p[5].y = node->miny; p[6].y = node->maxy; p[7].y = node->maxy;
	p[4].z = node->maxz; p[5].z = node->maxz; p[6].z = node->maxz; p[7].z = node->maxz;

	for(i = 0; i < 6; ++i) {
		int c = 8;
		for(j = 0; j < 8; ++j)
			if(n[i].a*p[j].x + n[i].b*p[j].y + n[i].c*p[j].z + n[i].d < 0) --c;
		if(c == 0) return 0;
	}
	return 1;
*/
}
static void octree_recurse(OCTNODE octnode, CAMERA camera, GALAXYLIST galaxies)
{
	if(intersects(octnode, camera)) {
//		printf("NODE [%lf %lf %lf][%lf %lf %lf]\n",
//					octnode->minx, octnode->miny, octnode->minz,
//					octnode->maxx, octnode->maxy, octnode->maxz);
		int i, size = galaxylist_size(octnode->galaxies);
		switch(size) {
			case -1:
				for(i = 0; i < 8; ++i)
					octree_recurse(octnode->child[i], camera, galaxies);
			break;
			default: {
//				printf("NODE [%lf %lf %lf][%lf %lf %lf]",
//					octnode->minx, octnode->miny, octnode->minz,
//					octnode->maxx, octnode->maxy, octnode->maxz);
//				printf(" %d\n", size);
				GALAXY *galarray = galaxylist_array(octnode->galaxies);
				for(i = 0; i < size; ++i)
					galaxylist_add(galaxies, galarray[i]);	
			} break;
		}
	}
}

void octree_galaxies(OCTNODE octnode, CAMERA camera, GALAXYLIST galaxylist)
{
	if(octnode == NULL) return;
	if(galaxylist == NULL) return;
	
//	double x = camera_positionX(camera);
//	double y = camera_positionY(camera);
//	double z = camera_positionZ(camera);
//	printf("CAMERA [%lf %lf %lf]\n", x, y, z);

//	{
//		int i;
//		plane *n = camera_planes(camera);
//		for(i = 0; i < 6; ++i)
//			printf("[%lf %lf %lf %lf]", n[i].a, n[i].b, n[i].c, n[i].d);
//		printf("\n");
//	}
//	return octnode->galaxies;

	galaxylist_reset(galaxylist);
	octree_recurse(octnode, camera, galaxylist);
}
