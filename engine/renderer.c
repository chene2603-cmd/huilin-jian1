#include "core/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG(msg) printf("[Renderer] %s\n", msg)

// 顶点着色器源码
static const char* vertex_shader_src = 
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 uModel;\n"
"uniform mat4 uView;\n"
"uniform mat4 uProj;\n"
"void main() {\n"
"    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);\n"
"}\n";

// 片段着色器源码
static const char* fragment_shader_src = 
"#version 330 core\n"
"uniform vec3 uColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(uColor, 1.0);\n"
"}\n";

// 编译着色器
static unsigned int compile_shader(const char* src, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        printf("Shader compile error: %s\n", info);
    }
    return shader;
}

// 初始化渲染器
void renderer_init(Renderer* r) {
    LOG("Initializing renderer");
    
    // 编译链接着色器
    unsigned int vs = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);
    unsigned int fs = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);
    
    r->shader_program = glCreateProgram();
    glAttachShader(r->shader_program, vs);
    glAttachShader(r->shader_program, fs);
    glLinkProgram(r->shader_program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    // ========== 1. 创建地面网格 (10x10棋盘格) ==========
    const int grid_size = 10;
    int vertex_count = grid_size * grid_size * 6;  // 每个格子2个三角形，每个三角形3个顶点
    float* grid_vertices = (float*)malloc(vertex_count * 3 * sizeof(float));
    int idx = 0;
    
    for (int ix = 0; ix < grid_size; ix++) {
        for (int iz = 0; iz < grid_size; iz++) {
            float x0 = (float)(ix - grid_size/2);
            float z0 = (float)(iz - grid_size/2);
            float x1 = x0 + 1.0f;
            float z1 = z0 + 1.0f;
            
            // 第一个三角形
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
            
            // 第二个三角形
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
        }
    }
    
    r->grid_vertex_count = vertex_count;
    
    glGenVertexArrays(1, &r->grid_vao);
    glGenBuffers(1, &r->grid_vbo);
    
    glBindVertexArray(r->grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), grid_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    free(grid_vertices);
    
    // ========== 2. 创建矩形 (1x1x1) ==========
    float rect_vertices[] = {
        // 前面
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &r->rect_vao);
    glGenBuffers(1, &r->rect_vbo);
    
    glBindVertexArray(r->rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // ========== 3. 创建三角形 ==========
    float tri_vertices[] = {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    glGenVertexArrays(1, &r->tri_vao);
    glGenBuffers(1, &r->tri_vbo);
    
    glBindVertexArray(r->tri_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->tri_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 解绑
    glBindVertexArray(0);
    
    LOG("Renderer initialized successfully");
}

// 绘制网格
void renderer_draw_grid(Renderer* r, const mat4* view, const mat4* proj) {
    glUseProgram(r->shader_program);
    
    // 传递视图和投影矩阵
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    // 单位模型矩阵
    mat4 model = mat4_identity();
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uModel"), 1, GL_FALSE, (float*)&model);
    
    glBindVertexArray(r->grid_vao);
    
    // 绘制每个格子并设置颜色
    int grid_size = 10;
    for (int ix = 0; ix < grid_size; ix++) {
        for (int iz = 0; iz < grid_size; iz++) {
            float brightness = ((ix + iz) % 2 == 0) ? 0.6f : 0.3f;
            glUniform3f(glGetUniformLocation(r->shader_program, "uColor"), 
                       brightness, brightness, brightness);
            glDrawArrays(GL_TRIANGLES, (ix * grid_size + iz) * 6, 6);
        }
    }
}

// 绘制矩形
void renderer_draw_rect(Renderer* r, float x, float y, float z, float w, float h, 
                       float red, float green, float blue, const mat4* view, const mat4* proj) {
    glUseProgram(r->shader_program);
    
    // 传递视图和投影矩阵
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    // 构建模型矩阵（缩放 + 平移）
    mat4 scale = {0};
    scale.m[0][0] = w;
    scale.m[1][1] = h;
    scale.m[2][2] = 1.0f;
    scale.m[3][3] = 1.0f;
    
    mat4 translate = {0};
    translate.m[0][0] = 1.0f;
    translate.m[1][1] = 1.0f;
    translate.m[2][2] = 1.0f;
    translate.m[3][3] = 1.0f;
    translate.m[3][0] = x;
    translate.m[3][1] = y;
    translate.m[3][2] = z;
    
    mat4 model = mat4_mul(translate, scale);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uModel"), 1, GL_FALSE, (float*)&model);
    
    // 设置颜色
    glUniform3f(glGetUniformLocation(r->shader_program, "uColor"), red, green, blue);
    
    // 绘制
    glBindVertexArray(r->rect_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// 绘制三角形
void renderer_draw_triangle(Renderer* r, const mat4* view, const mat4* proj) {
    glUseProgram(r->shader_program);
    
    // 传递视图和投影矩阵
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    // 模型矩阵（轻微抬高并放大）
    mat4 model = {0};
    model.m[0][0] = 2.0f;
    model.m[1][1] = 2.0f;
    model.m[2][2] = 2.0f;
    model.m[3][3] = 1.0f;
    model.m[3][1] = 0.5f;  // 抬高一点
    
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uModel"), 1, GL_FALSE, (float*)&model);
    
    // 设置颜色（使用顶点颜色插值）
    glUniform3f(glGetUniformLocation(r->shader_program, "uColor"), 1.0f, 1.0f, 1.0f);
    
    // 绘制
    glBindVertexArray(r->tri_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// 清理资源
void renderer_shutdown(Renderer* r) {
    glDeleteVertexArrays(1, &r->grid_vao);
    glDeleteBuffers(1, &r->grid_vbo);
    glDeleteVertexArrays(1, &r->rect_vao);
    glDeleteBuffers(1, &r->rect_vbo);
    glDeleteVertexArrays(1, &r->tri_vao);
    glDeleteBuffers(1, &r->tri_vbo);
    glDeleteProgram(r->shader_program);
    LOG("Renderer shutdown");
}