#ifndef CORE_CAMERA_H
#define CORE_CAMERA_H

#include "core/math.h"
#include <stdbool.h>

typedef struct {
    // 位置与朝向
    vec3 position;
    vec3 front;      // 摄像机前轴（看向的方向）
    vec3 up;         // 摄像机上轴
    vec3 right;      // 摄像机右轴
    vec3 worldUp;    // 世界空间的上方向，默认为(0,1,0)
    
    // 欧拉角 (弧度制)
    float yaw;       // 偏航角，绕世界Y轴旋转
    float pitch;     // 俯仰角，绕右轴旋转
    
    // 控制参数
    float moveSpeed;
    float mouseSensitivity;
} Camera;

// 初始化摄像机，设置默认位置和参数
void camera_init(Camera* cam);

// 根据键盘输入(WASD)更新摄像机位置
void camera_update(Camera* cam, float delta_time, bool w, bool a, bool s, bool d);

// 根据鼠标移动偏移量更新摄像机朝向
void camera_process_mouse(Camera* cam, float x_offset, float y_offset);

// 获取当前摄像机的视图矩阵（用于传递给着色器或glLoadMatrixf）
mat4 camera_get_view_matrix(const Camera* cam);

#endif
