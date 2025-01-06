#pragma once

#define SWI_FUNCTIONS_CNT 0x1000

#ifdef __cplusplus
extern "C" {
#endif

void loader_init_swilib();
void loader_swi_stub(int swi);
void loader_install_swihook();
void loader_uninstall_swihook();

extern void *SWILIB[];

#ifdef __cplusplus
}
#endif
