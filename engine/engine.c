#include "engine/engine.h"
#include <stdio.h>  /* 新增 */

#define LOG(msg) printf("[Huilin] %s\n", msg)  /* 新增 */

static bool running = false;

void engine_init(void) {
    LOG("Engine init");  /* 新增 */
    running = true;
}

void engine_update(void) {
    running = false;
}

void engine_shutdown(void) {
    LOG("Engine shutdown");  /* 新增 */
}