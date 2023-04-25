#pragma once

#define FATAL(MSG) { printf("\033[0;31m"); printf("FATAL: "); printf(MSG); printf("\033[0m\n"); abort(); }
#define ERROR(MSG) { printf("\033[0;31m"); printf("ERROR: "); printf(MSG); printf("\033[0m\n"); }
#define INFO(MSG) { printf("INFO: "); printf(MSG); printf("\n"); }

#ifndef NDEBUG
  #define DEBUG(MSG) { printf("DEBUG: "); printf(MSG); printf("\n"); }
#elif
  #define DEBUG(MSG)
#endif

