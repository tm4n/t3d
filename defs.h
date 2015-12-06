#ifndef __DEFS_H__
#define __DEFS_H__

#define GLM_FORCE_PURE

#include "vec.h"
#include "stdio.h"

// General definitions, types

#ifdef _WIN32
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#endif

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;


#endif // __DEFS_H__
