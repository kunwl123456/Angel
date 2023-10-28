#ifndef BASE_OS_DEF_H
#define BASE_OS_DEF_H

#if defined(__GNUC__)
#define TOS_UNUSED_ARG(a) ((void)a) //do{}while(NULL = &a)
#else
	#define TOS_UNUSED_ARG(a) a
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#include <endian.h>
#include <libgen.h>
#endif//defined(_WIN32) || defined(_WIN64)

#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#if defined(LITTLEENDIAN)&&LITTLEENDIAN > 0
#define TOS_LITTLEENDIAN

#if defined (TOS_BIGENDIAN)
#undef TOS_BIGENDIAN
#endif //#if defined (TOS_BIGENDIAN)
#else
#define TOS_BIGENDIAN
#if defined(TOS_LITTLEENDIAN)
#undef TOS_LITTLEENDIAN
#endif
#endif //#if defined(LITTLEENDIAN)&&LITTLEENDIAN > 0

#else //defined(_WIN32) || defined(_WIN64)
	//lack

#endif

#if defined(_WIN32) || defined(_WIN64)
#define TOS_DIRSEP  '\\'
#else
#define TOS_DIRSEP  '/'
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
//lack
#define mkdir(path,mode)   _mkdir(path)
#define getpid()           (pid_t) GetCurrentProcessId()
#else
#define MAX_PATH           PATH_MAX
#endif

#if defined(_WIN32)|| defined(_WIN64)
	void usleep(unsigned long usec);
#endif

#ifdef __cplusplus
}
#endif

#endif //BASE_OS_DEF_H
