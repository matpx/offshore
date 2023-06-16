#pragma once

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define FATAL(...)                                                 \
  {                                                                \
    printf("\033[0;31m");                                          \
    printf("FATAL(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); \
    printf(__VA_ARGS__);                                           \
    printf("\033[0m\n");                                           \
    assert(false);                                                 \
    abort();                                                       \
  }
#define LOG_ERROR(...)                                             \
  {                                                                \
    printf("\033[0;31m");                                          \
    printf("ERROR(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); \
    printf(__VA_ARGS__);                                           \
    printf("\033[0m\n");                                           \
  }
#define LOG_INFO(...)                                             \
  {                                                               \
    printf("INFO(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); \
    printf(__VA_ARGS__);                                          \
    printf("\n");                                                 \
  }

#ifndef NDEBUG
#define LOG_DEBUG(...)                                             \
  {                                                                \
    printf("\033[0;36m");                                          \
    printf("DEBUG(%s:%d:%s): ", __FILENAME__, __LINE__, __func__); \
    printf(__VA_ARGS__);                                           \
    printf("\033[0m\n");                                           \
  }
#elif
#define LOG_DEBUG(...)
#endif
