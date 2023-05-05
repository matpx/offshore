#pragma once

#include <cstdio>
#include <cassert>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FATAL(MSG) { printf("\033[0;31m"); printf("FATAL(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); printf(MSG); printf("\033[0m\n"); assert(false); abort(); }
#define LOG_ERROR(MSG) { printf("\033[0;31m"); printf("ERROR(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); printf(MSG); printf("\033[0m\n"); }
#define LOG_INFO(MSG)  { printf("INFO(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); printf(MSG); printf("\n"); }

#ifndef NDEBUG
#define LOG_DEBUG(MSG) { printf("\033[0;36m"); printf("DEBUG(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); printf(MSG); printf("\033[0m\n"); }
#elif
#define LOG_DEBUG(MSG)
#endif

