
#ifndef __ENV_H__
#define __ENV_H__

int loader_setenv(const char *name, const char *value, int replace);
int loader_unsetenv(const char *name);
char *loader_getenv(const char *var);
int loader_clearenv(void);

#endif
