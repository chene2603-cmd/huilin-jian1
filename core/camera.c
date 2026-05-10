#include "core/camera.h"
#include <math.h>

void camera_init(Camera* cam) {
    cam->position = (vec3){0.0f, 1.0f, 3.0f};
    cam->front    = (vec3){0.0f, 0.0f, -1.0f};
    cam->up       = (vec3){0.0f, 1.0f, 0.0f};
    cam->speed    = 2.5f;   // 米/秒
}

void camera_update(Camera* cam, float delta_time, bool w, bool a, bool s, bool d) {
    float velocity = cam->speed * delta_time;
    if (w) cam->position = vec3_add(cam->position, vec3_scale(cam->front, velocity));
    if (s) cam->position = vec3_sub(cam->position, vec3_scale(cam->front, velocity));
    if (a) {
        vec3 left = vec3_cross(cam->front, cam->up);
        left = vec3_normalize(left);
        cam->position = vec3_sub(cam->position, vec3_scale(left, velocity));
    }
    if (d) {
        vec3 right = vec3_cross(cam->front, cam->up);
        right = vec3_normalize(right);
        cam->position = vec3_add(cam->position, vec3_scale(right, velocity));
    }
}

mat4 camera_get_view_matrix(const Camera* cam) {
    vec3 center = vec3_add(cam->position, cam->front);
    return mat4_lookat(cam->position, center, cam->up);
}