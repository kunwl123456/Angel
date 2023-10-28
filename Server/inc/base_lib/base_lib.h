#ifndef _BASE_LIB_H__
#define _BASE_LIB_H__

#if defined(WIN32)|(WIN64)
#define _CRT_RAND_S
#define _MSVCVER (_MSC_VER)
#endif //WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <map>
#include <set>
#include <string>
#include <time.h>
#include <random>
#include <algorithm>
#include <immintrin.h>
#include <limits.h>


#include "wbus.h"

#if defined(WIN32) || defined (WIN64)
#include <direct.h>
#include <signal.h>
#define  TSIGNAL_BASIC
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif //WIN32 ||WIN64

#include "def/base_io_def.h"
#include "def/base_os_def.h"
#include "def/base_signal_def.h"

/*
#include "G3D/vectorMath.h"

typedef G3D::Vector3 AGVec3;
typedef G3D::Vector2 AGVec2;
*/

#endif //_BASE_LIB_H__