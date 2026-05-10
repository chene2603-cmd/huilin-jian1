#ifndef ENGINE_H
#define ENGINE_H

#include "core/core_types.h"

void  engine_init(void);
void  engine_update(void);
void  engine_shutdown(void);
bool  engine_is_running(void);

/* 新增：获取窗口指针，将来渲染用 */
void* engine_get_window(void);

#endif /* ENGINE_H */