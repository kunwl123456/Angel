#ifndef __TYPES_DEF_H_
#define __TYPES_DEF_H_

#include <stddef.h>
#include <time.h>
#include <sys/types.h>

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif
typedef int BOOL;


#if !defined(_WIN32) && !defined(_WIN64)
#include <stdint.h>
#include <inttypes.h>

#undef PRIdPTR
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)
#define PRIdPTR "lu"
#else
#define PRIdPTR "u"
#endif

#undef PRIu64
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)
#define PRIu64 "lu"
#else
#define PRIu64 "llu"
#endif
#else
	typedef signed    char    int8_t;
	typedef signed    short   int16_t;
	typedef signed    int     int32_t;
	typedef unsigned  char    uint8_t;
	typedef unsigned  short   uint16_t;
	typedef unsigned  int     uint32_t;

	typedef signed	  __int64 int64_t;
	typedef unsigned  __int64 uint64_t;

#endif

#if defined(_WIN32) || defined(_WIN64)
	#include <wchar.h>
	#include <limits.h>
#define  pid_t DWORD
	#if _MSC_VER >= 1400
		#include <crtdefs.h>
	#else
		typedef int  intptr_t;
		typedef unsigned long ULONG_PTR;
		typedef ULONG_PTR     *LPULONG_PTR;
		typedef ULONG_PTR     *PULONG_PTR;
		typedef ULONG_PTR     DWORD_PTR;
		typedef ULONG_PTR     *LPDWORD_PTR;
		typedef ULONG_PTR     *PDWORD_PTR;
	#endif

#endif

template<typename T> inline T angel_min(T a, T b) { return a < b ? a : b; }
template<typename T> inline T angel_max(T a, T b) { return a > b ? a : b; }
template<typename T> inline T angel_abs(T a) { return a < 0 ? -a : a; }


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

template <class T>
void SWAP(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = temp;
}

#define FLT_EPS  (1.192092896e-04F)
#define FLT_BZERO  (-FLT_EPS)
#define FLT_HZERO  (FLT_EPS)
//Ç÷½üÓÚ0
#define FLT_ZERO(x)   (x>= FLT_BZERO && x <= FLT_HZERO)

#ifndef ARRAYSIZE
#define ARRAYSIZ(a) (sizeof(a) / sizeof(*(a)))
#endif

template<typename T>inline T angle_min(T a, T b) { return a < b ?  a : b; }
template<typename T>inline T angle_max(T a, T b) { return a > b ? a : b; }
template<typename T>inline T angle_abs(T a) { return a < 0 ? -a : a; }


struct NETTIMERSTAP
{
	uint64_t	llSec;
	uint64_t	dwUSec;
	uint64_t	dwCumm;
	BOOL		iIsAvailable;
};

#endif
