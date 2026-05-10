#ifndef ENGINE_H
#define ENGINE_H

#include "core/core_types.h"

/* 引擎生命周期 */
void engine_init(void);
void engine_update(void);
void engine_shutdown(void);
bool engine_is_running(void);

#endif /* ENGINE_H */