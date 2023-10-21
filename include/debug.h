#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <common.h>

#define Log(format, ...) \
  printf("\33[1;33m[%9s]\33[0m \33[1;32m" format "\33[0m\n", \
      strrchr(__FILE__, '/')+1, ## __VA_ARGS__)

#undef panic
#define panic(format, ...) \
  do { \
    Log("\33[1;31msystem panic: " format, ## __VA_ARGS__); \
    halt(1); \
  } while (0)

#ifdef assert
# undef assert
#endif

#define assert(cond) \
  do { \
    if (!(cond)) { \
      panic("Assertion failed: %s", #cond); \
    } \
  } while (0)

#define TODO() panic("please implement me")

#endif
