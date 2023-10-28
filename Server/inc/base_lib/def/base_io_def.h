#ifndef BASE_IO_DEF_H
#define BASE_IO_DEF_H

#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>
#include <stdarg.h>
#include <errno.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if defined(_WIN32) || defined(_WIN64)
#define L_ctermid    (9)
#define L_cuserid    (9)

#ifndef TMP_MAX
#define TMP_MAX            (238328)
#endif

#if defined(_WIN32)||defined(_WIN64)
#if _MSC_VER < 1400
#define snprintf           _snprintf
#define vsnprintf          _vsnprintf
#define snwprintf          _snwprintf
#define vsnwprintf         _vsnwprintf
#else
#define snprintf(dest,count,fmt,...)       _snprintf_s(dest,count,_TRUNCATE,fmt,__VA_ARGS__)
#define vsnprintf(dest,count,fmt,valist)   _vsnprintf_s(dest,count,_TRUNCATE,fmt,valist)
#define snwprintf                          _snwprintf
#define vsnwprintf                         _vsnwprintf
#endif  

#else //defined(_WIN32)||defined(_WIN64)  linux
#define snprintf              _snprintf
#define vsnprintf             _vsnprintf
#define snwprintf             _snwprintf
#define vsnwprintf            _vsnwprintf
#endif //defined(_WIN32)||defined(_WIN64)

//#define sys_errlist   _syserrlist
//#define sys_nerr      _sys_nerr
#else
#endif// defined(_WIN32) || defined(_WIN64)

#ifdef __cplusplus
}
#endif //__cplusplus


#endif //BASE_IO_DEF_H










