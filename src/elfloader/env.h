#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int loader_setenv(const char *name, const char *value, int replace);
int loader_unsetenv(const char *name);
char *loader_getenv(const char *var);
int loader_clearenv(void);

#ifdef __cplusplus
}
#endif
