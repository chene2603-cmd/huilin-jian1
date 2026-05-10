#include "engine/engine.h"
#include "core/camera.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

#define LOG(msg) printf("[Huilin] %s\n", msg)

static GLFWwindow* window = NULL;
static bool running = false;
static Camera camera;
static int width = 1280, height = 720;
static float delta_time = 0.0f;
static float last_frame = 0.0f;

// 鼠标状态变量
static float last_mouse_x = 0.0f;
static float last_mouse_y = 0.0f;
static bool  first_mouse = true;

void engine_init(void) {
    LOG("Engine init");

    if (!glfwInit()) {
        LOG("Failed to initialize GLFW");
        return;
    }

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
    glfwSwapInterval(1); // 开启垂直同步

    // === 关键：隐藏光标并锁定到窗口 ===
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 获取初始光标位置，避免第一帧跳跃
    double init_x, init_y;
    glfwGetCursorPos(window, &init_x, &init_y);
    last_mouse_x = (float)init_x;
    last_mouse_y = (float)init_y;
    first_mouse = true;
    // === 鼠标设置结束 ===

    // 初始化摄像机
    camera_init(&camera);

    // 开启深度测试，正确渲染3D重叠物体
    glEnable(GL_DEPTH_TEST);
    // 设置视口
    glViewport(0, 0, width, height);

    last_frame = (float)glfwGetTime();
    running = true;
    LOG("Window created — 汴京第一扇门已打开");
    LOG("控制说明: WASD移动，鼠标环顾");
}

void engine_update(void) {
    // 计算每帧时间差
    float current_frame = (float)glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // 处理系统事件
    glfwPollEvents();

    // 检查窗口关闭请求
    if (glfwWindowShouldClose(window)) {
        running = false;
        return;
    }

    // ========== 1. 处理键盘输入 (WASD) ==========
    bool w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool a_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool s_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool d_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    // ========== 2. 处理鼠标输入 (视角环顾) ==========
    double current_mouse_x, current_mouse_y;
    glfwGetCursorPos(window, &current_mouse_x, &current_mouse_y);
    float xpos = (float)current_mouse_x;
    float ypos = (float)current_mouse_y;

    if (first_mouse) {
        // 跳过第一帧的鼠标数据，避免视角跳变
        last_mouse_x = xpos;
        last_mouse_y = ypos;
        first_mouse = false;
    }

    // 计算鼠标自上一帧的偏移量
    float x_offset = xpos - last_mouse_x;
    float y_offset = last_mouse_y - ypos; // Y轴取反，符合屏幕坐标直觉
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    // 将偏移量传递给摄像机处理
    camera_process_mouse(&camera, x_offset, y_offset);

    // ========== 3. 根据输入更新摄像机位置 ==========
    camera_update(&camera, delta_time, w_pressed, a_pressed, s_pressed, d_pressed);

    // ========== 4. 渲染场景 ==========
    // 清空颜色和深度缓冲区
    glClearColor(0.76f, 0.60f, 0.42f, 1.0f); // 土黄色背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置投影矩阵 (透视投影)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect_ratio = (float)width / (float)height;
    mat4 projection = mat4_perspective(45.0f, aspect_ratio, 0.1f, 100.0f);
    glLoadMatrixf((const float*)&projection);

    // 设置模型视图矩阵 (摄像机视图)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mat4 view = camera_get_view_matrix(&camera);
    glLoadMatrixf((const float*)&view);

    // 绘制一个彩色三角形作为视觉参考物
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  0.5f, 0.0f); // 红色顶点
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f); // 绿色顶点
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 0.5f, -0.5f, 0.0f); // 蓝色顶点
    glEnd();

    // 交换前后缓冲区
    glfwSwapBuffers(window);
}

void engine_shutdown(void) {
    LOG("Engine shutdown");
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