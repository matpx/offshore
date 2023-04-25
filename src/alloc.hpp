#pragma once

#include <stddef.h>

void *omalloc(size_t);
void ofree(void*);
void debug_leak_check();

