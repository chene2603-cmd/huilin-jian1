#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ========== 坐标系统（蓝图：架构总规范） ========== */
typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    int32_t x, z;       /* 世界网格坐标 */
} GridCoord;

/* ========== 资源ID（蓝图：架构总规范） ========== */
typedef uint32_t ResID;     /* 全局唯一资源标识符 */

/* ========== 实体基础类型 ========== */
typedef uint32_t EntityID;

typedef struct {
    EntityID    id;
    Vector3     position;
    ResID       model_id;
    bool        active;
} Entity;

#endif /* CORE_TYPES_H */