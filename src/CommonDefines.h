/*
============================================================================
AgmdEngine Define
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#ifndef _COMMONDEFINES_H_
#define _COMMOMDEFINES_H_

#define ENGINE_REV      3
#define ENGINE_NAME     "AgmdEngine"
#define ENGINE_STATE    "Alpha"

#define AGMD_MAJOR      0
#define AGMD_MINOR      2


#if _WIN32 || _WIN64
#define __WIN_PLATFORM__
#endif

#ifdef _DISABLE_WARNING_
_DEPRECATED_(name) name
#else
#ifdef GCC
#define _DEPRECATED_(name) __attribute__((deprecated)) name
#else
#define _DEPRECATED_(name) _CRT_DEPRECATE_TEXT("This function \"" #name "\" is deprecated, you may not use it !") name
#endif
#endif


typedef long long           int64;
typedef int                 int32;
typedef short int           int16;
typedef char                int8;
typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;



#endif /* _COMMOMDEFINES_H_ */