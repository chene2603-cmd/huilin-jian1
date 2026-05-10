#include "core/camera.h"
#include <math.h>

// 根据更新后的欧拉角(yaw, pitch)，重新计算前、右、上三个轴向向量
static void update_camera_vectors(Camera* cam) {
    // 1. 计算新的前向向量 (使用球坐标转换)
    vec3 new_front;
    new_front.x = cosf(cam->yaw) * cosf(cam->pitch);
    new_front.y = sinf(cam->pitch);
    new_front.z = sinf(cam->yaw) * cosf(cam->pitch);
    
    cam->front = vec3_normalize(new_front);
    
    // 2. 重新计算右向量和上向量
    cam->right = vec3_normalize(vec3_cross(cam->front, cam->worldUp));
    cam->up    = vec3_normalize(vec3_cross(cam->right, cam->front));
}

void camera_init(Camera* cam) {
    // 初始位置：在原点后方一点，高度为1（模拟人眼高度）
    cam->position = (vec3){0.0f, 1.0f, 3.0f};
    cam->worldUp  = (vec3){0.0f, 1.0f, 0.0f};
    
    // 初始朝向：看向-Z轴方向，对应yaw=-90度（转换为弧度）
    cam->yaw   = -90.0f * ((float)M_PI / 180.0f);
    cam->pitch = 0.0f;
    
    // 控制参数
    cam->moveSpeed = 2.5f;
    cam->mouseSensitivity = 0.001f; // 灵敏度较低，控制更精细
    
    // 初始化轴向向量
    update_camera_vectors(cam);
}

void camera_update(Camera* cam, float delta_time, bool w, bool a, bool s, bool d) {
    float velocity = cam->moveSpeed * delta_time;
    vec3 move_vec = {0.0f, 0.0f, 0.0f};
    
    // 根据按键组合出在XZ平面（水平面）的移动意向
    if (w) move_vec = vec3_add(move_vec, cam->front);
    if (s) move_vec = vec3_sub(move_vec, cam->front);
    if (a) move_vec = vec3_sub(move_vec, cam->right);
    if (d) move_vec = vec3_add(move_vec, cam->right);
    
    // 将Y分量置零，确保移动只发生在水平面，不“飞”起来
    move_vec.y = 0.0f;
    
    // 如果存在移动意向，则归一化，确保斜向移动速度与轴向一致
    float move_len = sqrtf(move_vec.x * move_vec.x + move_vec.z * move_vec.z);
    if (move_len > 0.001f) {
        move_vec.x /= move_len;
        move_vec.z /= move_len;
        cam->position = vec3_add(cam->position, vec3_scale(move_vec, velocity));
    }
}

void camera_process_mouse(Camera* cam, float x_offset, float y_offset) {
    // 应用鼠标灵敏度
    x_offset *= cam->mouseSensitivity;
    y_offset *= cam->mouseSensitivity;
    
    // 更新欧拉角
    cam->yaw   += x_offset;
    cam->pitch += y_offset;
    
    // 限制俯仰角，防止摄像机翻转（产生上下颠倒的奇怪视角）
    const float max_pitch_rad = 89.0f * ((float)M_PI / 180.0f);
    if (cam->pitch > max_pitch_rad) {
        cam->pitch = max_pitch_rad;
    }
    if (cam->pitch < -max_pitch_rad) {
        cam->pitch = -max_pitch_rad;
    }
    
    // 根据新的欧拉角更新前、右、上向量
    update_camera_vectors(cam);
}

mat4 camera_get_view_matrix(const Camera* cam) {
    vec3 center = vec3_add(cam->position, cam->front);
    return mat4_lookat(cam->position, center, cam->up);
}