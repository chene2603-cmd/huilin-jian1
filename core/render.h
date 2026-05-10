#ifndef CORE_RENDER_H
#define CORE_RENDER_H

#include "core/math.h"

typedef struct {
    unsigned int shader_program;    // 着色器程序
    unsigned int grid_vao, grid_vbo;  // 网格VAO/VBO
    unsigned int rect_vao, rect_vbo;  // 矩形VAO/VBO
    unsigned int tri_vao, tri_vbo;    // 三角形VAO/VBO
    int grid_vertex_count;            // 网格顶点数
} Renderer;

// 初始化渲染器
void renderer_init(Renderer* r);

// 绘制函数
void renderer_draw_grid(Renderer* r, const mat4* view, const mat4* proj);
void renderer_draw_rect(Renderer* r, float x, float y, float z, float w, float h, 
                        float r, float g, float b, const mat4* view, const mat4* proj);
void renderer_draw_triangle(Renderer* r, const mat4* view, const mat4* proj);

// 清理资源
void renderer_shutdown(Renderer* r);

#endif