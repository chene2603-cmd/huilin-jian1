#ifndef CORE_CAMERA_H
#define CORE_CAMERA_H

#include "core/math.h"
#include <stdbool.h>

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    float speed;
} Camera;

void camera_init(Camera* cam);
void camera_update(Camera* cam, float delta_time, bool w, bool a, bool s, bool d);
mat4 camera_get_view_matrix(const Camera* cam);

#endif