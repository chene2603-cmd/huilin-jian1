#ifndef CORE_RENDER_H
#define CORE_RENDER_H

#include "core/math.h"

typedef struct {
    // 主着色器
    unsigned int shader_program;
    unsigned int sky_shader;      // 天空盒着色器
    unsigned int axis_shader;     // 坐标轴着色器
    
    // 网格数据
    unsigned int grid_vao, grid_vbo;
    unsigned int rect_vao, rect_vbo;
    unsigned int sky_vao, sky_vbo;
    unsigned int axis_vao, axis_vbo;
    
    // 顶点计数
    int grid_vertex_count;
    int sky_vertex_count;
    
    // 光照参数
    vec3 light_dir;
    float ambient_strength;
} Renderer;

// 初始化渲染器
void renderer_init(Renderer* r);

// 绘制函数
void renderer_draw_grid(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_rect(Renderer* r, float x, float y, float z, float w, float h, 
                       float r, float g, float b, const mat4* view, const mat4* proj);
void renderer_draw_skybox(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_axes(Renderer* r, const mat4* view, const mat4* proj);

// 光照设置
void renderer_set_light_dir(Renderer* r, float x, float y, float z);
void renderer_set_ambient(Renderer* r, float ambient);

// 清理资源
void renderer_shutdown(Renderer* r);

#endif
