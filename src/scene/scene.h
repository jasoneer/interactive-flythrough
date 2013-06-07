#pragma once

#define NUM_TYPES 4247

#ifdef __cplusplus
extern "C" {
#endif

void scene_load(void);

void scene_toggle(void);
void scene_reset(void);

void scene_init(void);

void scene_resize(int width, int height);

void scene_update(float delta);

void scene_render(float delta);

#ifdef __cplusplus
}
#endif
