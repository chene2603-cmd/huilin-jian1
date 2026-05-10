#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <json-c/json.h>

#ifdef __cplusplus
extern "C" {
#endif

struct json_object* json_parse_file(const char* path);
struct json_object* json_parse_string(const char* str);

struct json_object* json_get_object(struct json_object* parent, const char* key);
double json_get_number(struct json_object* parent, const char* key);
const char* json_get_string(struct json_object* parent, const char* key);

int json_array_length(struct json_object* array);
struct json_object* json_array_get(struct json_object* array, int index);

/* 从 JSON 数组中读取 vec3（[x,y,z] 格式） */
int json_get_vec3(struct json_object* parent, const char* key,
                  float* out_x, float* out_y, float* out_z);

void json_free(struct json_object* root);

#ifdef __cplusplus
}
#endif

#endif
