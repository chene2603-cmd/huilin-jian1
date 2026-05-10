#include "engine/engine.h"

static bool running = false;

void engine_init(void) {
    running = true;
    /* 
     * 下一步在这里：
     * 1. 创建窗口
     * 2. 初始化OpenGL上下文
     * 3. 加载基础资源
     */
}

void engine_update(void) {
    /*
     * 下一步在这里：
     * 1. 处理输入事件
     * 2. 更新世界逻辑
     * 3. 渲染帧
     */
    running = false;  /* 暂时立即退出，避免死循环 */
}

void engine_shutdown(void) {
    /* 下一步在这里：清理资源 */
}

bool engine_is_running(void) {
    return running;
}