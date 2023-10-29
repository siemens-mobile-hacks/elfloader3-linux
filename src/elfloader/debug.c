#include <stdio.h>
#include <stddef.h>

#include "loader.h"

extern struct r_debug _r_debug;
static struct link_map *r_debug_tail = NULL;

static void _gdb_brk(void);

void loader_gdb_init() {
	struct link_map *cur = _r_debug.r_map;
	while (cur) {
		r_debug_tail = cur;
		cur = cur->l_next;
	}
}

Elf32_Word loader_gdb_r_debug() {
	return (Elf32_Word) &_r_debug;
}

void loader_gdb_add_lib(Elf32_Exec *ex) {
	_r_debug.r_state = RT_ADD;
	_gdb_brk();
	
	struct link_map *map = &ex->linkmap;
	map->l_addr = (Elf32_Addr) (ex->body->value - ex->v_addr);
	map->l_name = ex->fname;
	map->l_ld = (Elf32_Addr) ex->dynamic;
	
	if (r_debug_tail) {
		r_debug_tail->l_next = map;
		map->l_prev = r_debug_tail;
		map->l_next = 0;
	} else {
		_r_debug.r_map = map;
		map->l_prev = 0;
		map->l_next = 0;
	}
	r_debug_tail = map;
	
	_r_debug.r_state = RT_CONSISTENT;
	_gdb_brk();
}

void loader_gdb_remove_lib(Elf32_Exec *ex) {
	struct link_map *map = &ex->linkmap;
	
	if (r_debug_tail == map)
		r_debug_tail = map->l_prev;
	
	if (map->l_prev) map->l_prev->l_next = map->l_next;
	if (map->l_next) map->l_next->l_prev = map->l_prev;
}

static void _gdb_brk(void) {
	if (_r_debug.r_brk)
		_r_debug.r_brk();
}
