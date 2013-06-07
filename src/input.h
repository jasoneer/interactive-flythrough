#pragma once

typedef struct _input
{
	float velocity;

	float angle_x;
	float angle_y;
} input;

#ifdef __cplusplus
extern "C" {
#endif

input input_get(void);

const char *input_path(void);

char *input_path_concat(const char *filename);

char *concat(const char *left, const char *right);

#ifdef __cplusplus
}
#endif
