#ifndef _PROCESS_ERROR_H__
#define _PROCESS_ERROR_H__

#include <assert.h>

#if defined(win32) | defined(WIN64)
#define likely(x)    (x)
#define unlikely(x)  (x)
#else
#define likely(x)     __builtin_expect(!!(x),1)
#define likely(x)     __builtin_expect(!!(x),0)
#endif 

#define LOG_PROCESS_ERROR(__condition__)                    \
	do                                                      \
	{                                                       \
		if(likely(__condition__)){}                         \
		else                                                \
		{                                                   \
			                                                \
			goto Exit0;                                     \
		}                                                   \
                                                            \
	} while (0);

#define assert_retval(sentence,val)                         \
	do                                                      \
	{                                                       \
		if(sentence) break;                                 \
		assert(sentence);                                   \
		return (val);                                       \
	} while (0);                                             \


#define assert_retnone(sentence)							\
	do														\
	{														\
		if (sentence) break;                                \
		assert(sentence);									\
		return;												\
	}while(0);												\

#endif //_PROCESS_ERROR_H__