#pragma once

#include <cstdio>
#include <cassert>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FATAL(MSG) { printf("\033[0;31m"); printf("FATAL(%s:%s:%d): ", __FILENAME__, __func__, __LINE__); printf(MSG); printf("\033[0m\n"); assert(false); abort(); }
#define ERROR(MSG) { printf("\033[0;31m"); printf("ERROR(%s:%s:%d): ", __FILENAME__, __func__, __LINE__); printf(MSG); printf("\033[0m\n"); }
#define INFO(MSG) { printf("INFO(%s:%s:%d): ", __FILENAME__, __func__, __LINE__); printf(MSG); printf("\n"); }

#ifndef NDEBUG
  #define DEBUG(MSG) { printf("DEBUG(%s:%s:%d): ", __FILENAME__, __func__, __LINE__); printf(MSG); printf("\n"); }
#elif
  #define DEBUG(MSG)
#endif

