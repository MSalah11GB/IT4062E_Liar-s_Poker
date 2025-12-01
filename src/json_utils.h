#ifndef JSON_UTILS_H
#define JSON_UTILS_H
#include <cjson/cJSON.h>

cJSON* json_parse(const char *s);
char* json_build_simple(const char *type, const char *raw_json_object_text);
char* json_build_object(const char *type, cJSON *obj);
char* json_build_object_with_child(const char *type, const char *child_name, cJSON *child);
char* json_build_error(const char *errcode, const char *reason);
void send_raw_json(int sock, const char *s);

#endif
