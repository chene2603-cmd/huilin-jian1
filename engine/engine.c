#include "engine/engine.h"
#include "core/camera.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

#define LOG(msg) printf("[Huilin] %s\n", msg)

static GLFWwindow* window = NULL;
static bool running = false;
static Camera camera;                  // 新增
static int width = 1280, height = 720; // 新增，用于后续比例
static float delta_time = 0.0f;
static float last_frame = 0.0f;

void engine_init(void) {
    LOG("Engine init");

    if (!glfwInit()) {
        LOG("Failed to initialize GLFW");
        return;
    }

    // 使用兼容模式，以便使用固定管线渲染三角形
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
    glfwSwapInterval(1);

    // 初始化摄像机
    camera_init(&camera);

    // 开启深度测试，为3D做准备
    glEnable(GL_DEPTH_TEST);

    // 设置视口
    glViewport(0, 0, width, height);

    last_frame = (float)glfwGetTime();
    running = true;
    LOG("Window created — 汴京第一扇门已打开");
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

    // 处理输入
    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    // 更新摄像机
    camera_update(&camera, delta_time, w, a, s, d);

    // 渲染
    glClearColor(0.76f, 0.60f, 0.42f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置投影矩阵（固定管线风格）
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    mat4 proj = mat4_perspective(45.0f, aspect, 0.1f, 100.0f);
    glLoadMatrixf((const float*)&proj);

    // 设置视图矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mat4 view = camera_get_view_matrix(&camera);
    glLoadMatrixf((const float*)&view);

    // 绘制一个彩色三角形，用来验证摄像机移动
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 0.5f, -0.5f, 0.0f);
    glEnd();

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