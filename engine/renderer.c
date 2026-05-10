#include "core/render.h"
#include "core/scene.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOG(msg) printf("[Renderer] %s\n", msg)

// ========== 着色器源码 ==========
static const char* vertex_shader_src =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aNormal;\n"
    "uniform mat4 uModel;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProj;\n"
    "out vec3 vNormal;\n"
    "out vec3 vFragPos;\n"
    "void main() {\n"
    "    vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
    "    vFragPos = worldPos.xyz;\n"
    "    vNormal = mat3(transpose(inverse(uModel))) * aNormal;\n"
    "    gl_Position = uProj * uView * worldPos;\n"
    "}\n";

static const char* fragment_shader_src =
    "#version 330 core\n"
    "in vec3 vNormal;\n"
    "in vec3 vFragPos;\n"
    "uniform vec3 uColor;\n"
    "uniform vec3 uLightDir;\n"
    "uniform float uAmbient;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    vec3 normal = normalize(vNormal);\n"
    "    vec3 lightDir = normalize(-uLightDir);\n"
    "    float diff = max(dot(normal, lightDir), 0.0);\n"
    "    float lighting = uAmbient + (1.0 - uAmbient) * diff;\n"
    "    FragColor = vec4(uColor * lighting, 1.0);\n"
    "}\n";

// 天空盒着色器
static const char* sky_vertex_src =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProj;\n"
    "out vec3 vTexCoord;\n"
    "void main() {\n"
    "    vTexCoord = aPos;\n"
    "    mat4 viewNoTrans = mat4(mat3(uView));\n"
    "    vec4 pos = uProj * viewNoTrans * vec4(aPos, 1.0);\n"
    "    gl_Position = pos.xyww;\n"
    "}\n";

static const char* sky_fragment_src =
    "#version 330 core\n"
    "in vec3 vTexCoord;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    float h = normalize(vTexCoord).y;\n"
    "    vec3 topColor = vec3(0.4, 0.6, 0.9);\n"
    "    vec3 horizonColor = vec3(0.7, 0.8, 0.9);\n"
    "    vec3 groundColor = vec3(0.3, 0.3, 0.2);\n"
    "    float t = smoothstep(-0.1, 0.3, h);\n"
    "    vec3 sky = mix(horizonColor, topColor, t);\n"
    "    if (h < 0.0) {\n"
    "        sky = mix(groundColor, horizonColor, (h + 0.5) * 2.0);\n"
    "    }\n"
    "    FragColor = vec4(sky, 1.0);\n"
    "}\n";

// 坐标轴着色器
static const char* axis_vertex_src =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProj;\n"
    "out vec3 vColor;\n"
    "void main() {\n"
    "    vColor = aColor;\n"
    "    gl_Position = uProj * uView * vec4(aPos, 1.0);\n"
    "}\n";

static const char* axis_fragment_src =
    "#version 330 core\n"
    "in vec3 vColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(vColor, 1.0);\n"
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

// 创建着色器程序
static unsigned int create_program(const char* vs_src, const char* fs_src) {
    unsigned int vs = compile_shader(vs_src, GL_VERTEX_SHADER);
    unsigned int fs = compile_shader(fs_src, GL_FRAGMENT_SHADER);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// 生成球体顶点
static void generate_sphere_vertices(float radius, int stacks, int slices, 
                                     float** vertices, int* vertex_count) {
    int num_vertices = stacks * slices * 6;
    *vertices = (float*)malloc(num_vertices * 3 * sizeof(float));
    *vertex_count = num_vertices;
    
    int idx = 0;
    for (int i = 0; i < stacks; i++) {
        float phi1 = (float)M_PI * i / stacks;
        float phi2 = (float)M_PI * (i + 1) / stacks;
        
        for (int j = 0; j < slices; j++) {
            float theta1 = 2.0f * (float)M_PI * j / slices;
            float theta2 = 2.0f * (float)M_PI * (j + 1) / slices;
            
            float x1 = radius * sinf(phi1) * cosf(theta1);
            float y1 = radius * cosf(phi1);
            float z1 = radius * sinf(phi1) * sinf(theta1);
            
            float x2 = radius * sinf(phi2) * cosf(theta1);
            float y2 = radius * cosf(phi2);
            float z2 = radius * sinf(phi2) * sinf(theta1);
            
            float x3 = radius * sinf(phi2) * cosf(theta2);
            float y3 = radius * cosf(phi2);
            float z3 = radius * sinf(phi2) * sinf(theta2);
            
            float x4 = radius * sinf(phi1) * cosf(theta2);
            float y4 = radius * cosf(phi1);
            float z4 = radius * sinf(phi1) * sinf(theta2);
            
            float tri1[] = {x1, y1, z1, x2, y2, z2, x3, y3, z3};
            float tri2[] = {x1, y1, z1, x3, y3, z3, x4, y4, z4};
            
            memcpy(&(*vertices)[idx], tri1, sizeof(tri1));
            idx += 9;
            memcpy(&(*vertices)[idx], tri2, sizeof(tri2));
            idx += 9;
        }
    }
}

// 初始化渲染器
void renderer_init(Renderer* r) {
    LOG("Initializing renderer");
    
    if (glewInit() != GLEW_OK) {
        LOG("Failed to initialize GLEW in renderer");
        return;
    }
    
    r->shader_program = create_program(vertex_shader_src, fragment_shader_src);
    r->sky_shader = create_program(sky_vertex_src, sky_fragment_src);
    r->axis_shader = create_program(axis_vertex_src, axis_fragment_src);
    
    r->light_dir = (vec3){0.5f, -1.0f, -0.3f};
    r->ambient_strength = 0.2f;
    
    // ========== 1. 地面网格 (10x10棋盘格) ==========
    const int grid_size = 10;
    int vertex_count = grid_size * grid_size * 6;
    float* grid_vertices = (float*)malloc(vertex_count * 6 * sizeof(float));
    int idx = 0;
    
    for (int ix = 0; ix < grid_size; ix++) {
        for (int iz = 0; iz < grid_size; iz++) {
            float x0 = (float)(ix - grid_size/2);
            float z0 = (float)(iz - grid_size/2);
            float x1 = x0 + 1.0f;
            float z1 = z0 + 1.0f;
            
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
            
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
            
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
            
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z0;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
            
            grid_vertices[idx++] = x1; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
            
            grid_vertices[idx++] = x0; grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = z1;
            grid_vertices[idx++] = 0.0f; grid_vertices[idx++] = 1.0f; grid_vertices[idx++] = 0.0f;
        }
    }
    
    r->grid_vertex_count = vertex_count;
    
    glGenVertexArrays(1, &r->grid_vao);
    glGenBuffers(1, &r->grid_vbo);
    glBindVertexArray(r->grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, idx * sizeof(float), grid_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    free(grid_vertices);
    
    // ========== 2. 矩形 (1x1) ==========
    float rect_vertices[] = {
        0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        
        0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &r->rect_vao);
    glGenBuffers(1, &r->rect_vbo);
    glBindVertexArray(r->rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // ========== 3. 三角形VAO (新增) ==========
    float tri_vertices[] = {
        0.0f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &r->tri_vao);
    glGenBuffers(1, &r->tri_vbo);
    glBindVertexArray(r->tri_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->tri_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // ========== 4. 天空盒 (球体) ==========
    float* sky_vertices = NULL;
    generate_sphere_vertices(50.0f, 16, 32, &sky_vertices, &r->sky_vertex_count);
    
    glGenVertexArrays(1, &r->sky_vao);
    glGenBuffers(1, &r->sky_vbo);
    glBindVertexArray(r->sky_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->sky_vbo);
    glBufferData(GL_ARRAY_BUFFER, r->sky_vertex_count * 3 * sizeof(float), sky_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    free(sky_vertices);
    
    // ========== 5. 坐标轴 ==========
    float axis_vertices[] = {
        0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        10.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        
        0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.0f, 10.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 10.0f,  0.0f, 0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &r->axis_vao);
    glGenBuffers(1, &r->axis_vbo);
    glBindVertexArray(r->axis_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->axis_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    LOG("Renderer initialized successfully");
}

// 绘制网格
void renderer_draw_grid(Renderer* r, const mat4* view, const mat4* proj) {
    glUseProgram(r->shader_program);
    
    mat4 model = mat4_identity();
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uModel"), 1, GL_FALSE, (float*)&model);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    glUniform3f(glGetUniformLocation(r->shader_program, "uLightDir"), 
                r->light_dir.x, r->light_dir.y, r->light_dir.z);
    glUniform1f(glGetUniformLocation(r->shader_program, "uAmbient"), r->ambient_strength);
    
    glBindVertexArray(r->grid_vao);
    
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
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    glUniform3f(glGetUniformLocation(r->shader_program, "uLightDir"), 
                r->light_dir.x, r->light_dir.y, r->light_dir.z);
    glUniform1f(glGetUniformLocation(r->shader_program, "uAmbient"), r->ambient_strength);
    
    glUniform3f(glGetUniformLocation(r->shader_program, "uColor"), red, green, blue);
    
    glBindVertexArray(r->rect_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// 绘制三角形 (新增函数)
void renderer_draw_triangle(Renderer* r, float x, float y, float z, 
                           float sx, float sy, float red, float green, float blue,
                           const mat4* view, const mat4* proj) {
    glUseProgram(r->shader_program);
    
    mat4 scale = {0};
    scale.m[0][0] = sx;
    scale.m[1][1] = sy;
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
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->shader_program, "uProj"), 1, GL_FALSE, (float*)proj);
    
    glUniform3f(glGetUniformLocation(r->shader_program, "uLightDir"), 
                r->light_dir.x, r->light_dir.y, r->light_dir.z);
    glUniform1f(glGetUniformLocation(r->shader_program, "uAmbient"), r->ambient_strength);
    
    glUniform3f(glGetUniformLocation(r->shader_program, "uColor"), red, green, blue);
    
    glBindVertexArray(r->tri_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// 绘制天空盒
void renderer_draw_skybox(Renderer* r, const mat4* view, const mat4* proj) {
    glDepthMask(GL_FALSE);
    
    glUseProgram(r->sky_shader);
    
    mat4 view_no_trans = *view;
    view_no_trans.m[3][0] = 0.0f;
    view_no_trans.m[3][1] = 0.0f;
    view_no_trans.m[3][2] = 0.0f;
    
    glUniformMatrix4fv(glGetUniformLocation(r->sky_shader, "uView"), 1, GL_FALSE, (float*)&view_no_trans);
    glUniformMatrix4fv(glGetUniformLocation(r->sky_shader, "uProj"), 1, GL_FALSE, (float*)proj);
    
    glBindVertexArray(r->sky_vao);
    glDrawArrays(GL_TRIANGLES, 0, r->sky_vertex_count);
    
    glDepthMask(GL_TRUE);
}

// 绘制坐标轴
void renderer_draw_axes(Renderer* r, const mat4* view, const mat4* proj) {
    glUseProgram(r->axis_shader);
    
    mat4 model = mat4_identity();
    glUniformMatrix4fv(glGetUniformLocation(r->axis_shader, "uView"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(r->axis_shader, "uProj"), 1, GL_FALSE, (float*)proj);
    
    glBindVertexArray(r->axis_vao);
    
    for (int i = 0; i < 3; i++) {
        glDrawArrays(GL_LINES, i * 2, 2);
    }
}

// 场景渲染 (新增函数)
void renderer_draw_scene(Renderer* r, const Scene* scene, const mat4* view, const mat4* proj) {
    if (!scene || !r) return;
    
    for (int i = 0; i < scene->object_count; i++) {
        const SceneObject* obj = &scene->objects[i];
        
        switch (obj->type) {
            case SCENE_OBJ_RECT:
            case SCENE_OBJ_CUBE:
                renderer_draw_rect(r, 
                    obj->position.x, obj->position.y, obj->position.z,
                    obj->scale.x, obj->scale.y,
                    obj->color.x, obj->color.y, obj->color.z,
                    view, proj);
                break;
                
            case SCENE_OBJ_TRIANGLE:
                renderer_draw_triangle(r,
                    obj->position.x, obj->position.y, obj->position.z,
                    obj->scale.x, obj->scale.y,
                    obj->color.x, obj->color.y, obj->color.z,
                    view, proj);
                break;
                
            case SCENE_OBJ_GRID:
                renderer_draw_grid(r, view, proj);
                break;
                
            default:
                fprintf(stderr, "WARNING: Unknown object type: %d\n", obj->type);
                break;
        }
    }
}

// 设置光照方向
void renderer_set_light_dir(Renderer* r, float x, float y, float z) {
    r->light_dir = vec3_normalize((vec3){x, y, z});
}

// 设置环境光强度
void renderer_set_ambient(Renderer* r, float ambient) {
    r->ambient_strength = ambient;
}

// 清理资源
void renderer_shutdown(Renderer* r) {
    glDeleteVertexArrays(1, &r->grid_vao);
    glDeleteBuffers(1, &r->grid_vbo);
    glDeleteVertexArrays(1, &r->rect_vao);
    glDeleteBuffers(1, &r->rect_vbo);
    glDeleteVertexArrays(1, &r->tri_vao);
    glDeleteBuffers(1, &r->tri_vbo);
    glDeleteVertexArrays(1, &r->sky_vao);
    glDeleteBuffers(1, &r->sky_vbo);
    glDeleteVertexArrays(1, &r->axis_vao);
    glDeleteBuffers(1, &r->axis_vbo);
    glDeleteProgram(r->shader_program);
    glDeleteProgram(r->sky_shader);
    glDeleteProgram(r->axis_shader);
    LOG("Renderer shutdown");
}