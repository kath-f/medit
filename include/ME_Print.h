#ifndef __ME_PRINT_H
#define __ME_PRINT_H

/*
  ME_Print.h
  Defines printing related structures and functions
 */

#include <stdarg.h>

typedef enum{
  ME_LOGLEVEL_ERROR,
  ME_LOGLEVEL_WARN,
  ME_LOGLEVEL_INFO,
  ME_LOGLEVEL_DEBUG,
  ME_LOGLEVEL_NONE
}ME_LogLevel;

#ifndef ME_DEFAULT_LOGLEVEL
#define ME_DEFAULT_LOGLEVEL ME_LOGLEVEL_NONE
#endif

extern ME_LogLevel ME_GLOBAL_LOGLEVEL;

#define ME_SEPARATOR "------------------------------------------------------------------------"

void me_print(ME_LogLevel loglvl, const char* str, ...);

#endif
