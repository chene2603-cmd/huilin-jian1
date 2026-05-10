#include "engine/engine.h"
#include "core/camera.h"
#include "core/render.h"
#include "core/scene.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LOG(msg) printf("[Huilin] %s\n", msg)

static GLFWwindow* window = NULL;
static bool running = false;
static Camera camera;
static Renderer renderer;
static Scene current_scene;  // 新增
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
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        LOG("Failed to initialize GLEW");
        glfwTerminate();
        return;
    }
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double init_x, init_y;
    glfwGetCursorPos(window, &init_x, &init_y);
    last_mouse_x = (float)init_x;
    last_mouse_y = (float)init_y;
    first_mouse = true;
    
    camera_init(&camera);
    renderer_init(&renderer);
    
    // 初始化场景
    scene_clear(&current_scene);
    if (!scene_load(&current_scene, "scenes/test.json")) {
        LOG("WARNING: Failed to load scene, using fallback objects");
    }
    
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    
    last_frame = (float)glfwGetTime();
    running = true;
    LOG("汴京世界已就绪 - 数据驱动模式启动");
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 设置光照
    renderer_set_light_dir(&renderer, 0.5f, -1.0f, -0.3f);
    renderer_set_ambient(&renderer, 0.2f);
    
    // 1. 绘制天空盒
    renderer_draw_skybox(&renderer, &view, &proj);
    
    // 2. 绘制场景（完全数据驱动）
    renderer_draw_scene(&renderer, &current_scene, &view, &proj);
    
    // 3. 绘制坐标轴
    renderer_draw_axes(&renderer, &view, &proj);
    
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
