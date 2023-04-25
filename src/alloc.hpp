#pragma once

#ifndef NDEBUG
  #include <stb/stb_leakcheck.h>
  #define omalloc(N) stb_leakcheck_malloc(N, __FILE__, __LINE__);
  #define ofree(P) stb_leakcheck_free(P);
#elif
  #define omalloc(N) malloc(N);
  #define ofree(P) malloc(P);
#endif

