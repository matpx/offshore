#pragma once

#include <malloc.h>

#define OMALLOC(N) malloc(N);
#define OFREE(P) free(P);

