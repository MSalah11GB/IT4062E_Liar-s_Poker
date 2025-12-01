#include "json_utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// parse using cJSON
cJSON* json_parse(const char *s) {
    return cJSON_Parse(s);
}

char* json_build_simple(const char *type, const char *raw_json_object_text) {
    // raw_json_object_text should be like "{\"success\":true}"
    size_t n = strlen(type) + strlen(raw_json_object_text) + 64;
    char *buf = malloc(n);
    snprintf(buf, n, "{\"type\":\"%s\",%s}", type, raw_json_object_text+1); // raw assumed starting with {
    return buf;
}

char* json_build_object(const char *type, cJSON *obj) {
    char *body = cJSON_PrintUnformatted(obj);
    size_t n = strlen(type) + strlen(body) + 64;
    char *buf = malloc(n);
    snprintf(buf, n, "{\"type\":\"%s\",\"payload\":%s}", type, body);
    free(body);
    return buf;
}

char* json_build_object_with_child(const char *type, const char *child_name, cJSON *child) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, child_name, child);
    char *body = cJSON_PrintUnformatted(root);
    size_t n = strlen(type) + strlen(body) + 64;
    char *buf = malloc(n);
    snprintf(buf, n, "{\"type\":\"%s\",%s}", type, body+1); // embed without leading {
    free(body);
    cJSON_Delete(root); // child already added to body string; it's safe to delete
    return buf;
}

char* json_build_error(const char *errcode, const char *reason) {
    cJSON *o = cJSON_CreateObject();
    cJSON_AddStringToObject(o, "error", errcode);
    cJSON_AddStringToObject(o, "reason", reason);
    char *s = json_build_object("SYSTEM_ERROR", o);
    return s;
}

void send_raw_json(int sock, const char *s) {
    // sends newline-terminated JSON
    send(sock, s, strlen(s), 0);
    send(sock, "\n", 1, 0);
}
