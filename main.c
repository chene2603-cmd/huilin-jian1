#include "engine/engine.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    engine_init();
    
    /* 主循环：目前直接结束，等待你的窗口初始化代码 */
    while (engine_is_running()) {
        engine_update();
    }
    
    engine_shutdown();
    return 0;
}