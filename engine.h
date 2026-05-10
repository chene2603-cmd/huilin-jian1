#include "engine/engine.h"
#include <stdio.h>
#include <GLFW/glfw3.h>   /* 新增 */

#define LOG(msg) printf("[Huilin] %s\n", msg)

static GLFWwindow* window = NULL;  /* 新增 */
static bool running = false;

void engine_init(void) {
    LOG("Engine init");

    /* 初始化 GLFW */
    if (!glfwInit()) {
        LOG("Failed to initialize GLFW");
        return;
    }

    /* 创建窗口 */
    window = glfwCreateWindow(1280, 720, "汴京", NULL, NULL);
    if (!window) {
        LOG("Failed to create window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  /* 垂直同步 */

    running = true;
    LOG("Window created — 汴京第一扇门已打开");
}

void engine_update(void) {
    /* 轮询事件，处理关闭按钮 */
    glfwPollEvents();

    if (glfwWindowShouldClose(window)) {
        running = false;
        return;
    }

    /* 清屏为土黄色 — 汴京的土地 */
    glClearColor(0.76f, 0.60f, 0.42f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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