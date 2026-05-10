#ifndef CORE_RENDER_H
#define CORE_RENDER_H

#include "core/math.h"

typedef struct {
    unsigned int shader_program;
    unsigned int sky_shader;
    unsigned int axis_shader;
    
    unsigned int grid_vao, grid_vbo;
    unsigned int rect_vao, rect_vbo;
    unsigned int tri_vao, tri_vbo;    // 新增三角形VAO
    unsigned int sky_vao, sky_vbo;
    unsigned int axis_vao, axis_vbo;
    
    int grid_vertex_count;
    int sky_vertex_count;
    
    vec3 light_dir;
    float ambient_strength;
} Renderer;

void renderer_init(Renderer* r);
void renderer_draw_grid(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_rect(Renderer* r, float x, float y, float z, float w, float h, 
                       float r, float g, float b, const mat4* view, const mat4* proj);
void renderer_draw_triangle(Renderer* r, float x, float y, float z, 
                           float sx, float sy, float r, float g, float b,
                           const mat4* view, const mat4* proj);  // 新增
void renderer_draw_skybox(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_axes(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_scene(Renderer* r, const Scene* scene, const mat4* view, const mat4* proj);  // 新增
void renderer_set_light_dir(Renderer* r, float x, float y, float z);
void renderer_set_ambient(Renderer* r, float ambient);
void renderer_shutdown(Renderer* r);

#endif