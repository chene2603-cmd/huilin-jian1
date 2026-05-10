#include "engine/engine.h"
#include "core/camera.h"
#include "core/render.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LOG(msg) printf("[Huilin] %s\n", msg)

static GLFWwindow* window = NULL;
static bool running = false;
static Camera camera;
static Renderer renderer;
static int width = 1280, height = 720;
static float delta_time = 0.0f;
static float last_frame = 0.0f;
static float last_mouse_x = 0.0f;
static float last_mouse_y = 0.0f;
static bool first_mouse = true;

void engine_init(void) {
    LOG("Engine init");
    
    if (!glfwInit()) {
        LOG("Failed to initialize GLFW");
        return;
    }
    
    // 使用兼容模式，以便保留立即模式用于调试
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    
    window = glfwCreateWindow(width, height, "汴京", NULL, NULL);
    if (!window) {
        LOG("Failed to create window");
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // 开启垂直同步
    
    // 初始化GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        LOG("Failed to initialize GLEW");
        glfwTerminate();
        return;
    }
    
    // 设置鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double init_x, init_y;
    glfwGetCursorPos(window, &init_x, &init_y);
    last_mouse_x = (float)init_x;
    last_mouse_y = (float)init_y;
    first_mouse = true;
    
    // 初始化摄像机
    camera_init(&camera);
    
    // 初始化渲染器
    renderer_init(&renderer);
    
    // OpenGL设置
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    
    last_frame = (float)glfwGetTime();
    running = true;
    LOG("汴京世界已就绪 - WASD移动，鼠标环顾");
}

void engine_update(void) {
    float current_frame = (float)glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    
    glfwPollEvents();
    
    if (glfwWindowShouldClose(window)) {
        running = false;
        return;
    }
    
    // 键盘输入
    bool w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool a_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool s_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool d_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    
    // 鼠标输入
    double current_mouse_x, current_mouse_y;
    glfwGetCursorPos(window, &current_mouse_x, &current_mouse_y);
    float xpos = (float)current_mouse_x;
    float ypos = (float)current_mouse_y;
    
    if (first_mouse) {
        last_mouse_x = xpos;
        last_mouse_y = ypos;
        first_mouse = false;
    }
    
    float x_offset = xpos - last_mouse_x;
    float y_offset = last_mouse_y - ypos;
    last_mouse_x = xpos;
    last_mouse_y = ypos;
    
    camera_process_mouse(&camera, x_offset, y_offset);
    camera_update(&camera, delta_time, w_pressed, a_pressed, s_pressed, d_pressed);
    
    // 获取矩阵
    mat4 view = camera_get_view_matrix(&camera);
    float aspect_ratio = (float)width / (float)height;
    mat4 proj = mat4_perspective(45.0f, aspect_ratio, 0.1f, 100.0f);
    
    // 渲染
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);  // 天蓝色背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 绘制地面
    renderer_draw_grid(&renderer, &view, &proj);
    
    // 绘制建筑占位符
    renderer_draw_rect(&renderer, -3.0f, 0.5f, -3.0f, 1.0f, 1.0f, 
                      0.8f, 0.4f, 0.2f, &view, &proj);  // 小房子
    
    renderer_draw_rect(&renderer, 2.0f, 1.0f, -2.0f, 1.5f, 2.0f, 
                      0.6f, 0.3f, 0.3f, &view, &proj);  // 红色大建筑
    
    renderer_draw_rect(&renderer, -2.0f, 0.3f, 3.0f, 2.0f, 0.6f, 
                      0.2f, 0.6f, 0.4f, &view, &proj);  // 紫色长建筑
    
    // 用立即模式绘制三角形（调试用）
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)&proj);
    glMatrixMode(GL_MODELVIEW);
    mat4 mv = mat4_mul(view, mat4_identity());
    glLoadMatrixf((float*)&mv);
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.6f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-0.3f, -0.3f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.3f, -0.3f, 0.0f);
    glEnd();
    
    glfwSwapBuffers(window);
}

void engine_shutdown(void) {
    LOG("Engine shutdown");
    renderer_shutdown(&renderer);
    if (window) {
        glfwDestroyWindow(window);
        window = NULL;
    }
    glfwTerminate();
}

bool engine_is_running(void) {
    return running;
}

void* engine_get_window(void) {
    return (void*)window;
}