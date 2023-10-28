#ifndef TWAIT_H
#define TWAIT_H

#include "def/type_def.h"

#if defined(_WIN32) || defined(_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif

/* BITS in third argument to 'waitpid' .*/
#define WNOHJANG     1		/* don't block waiting*/
#define WUNTRACED    2		/* report status of stopped children */

/* BITS in four argument to 'waitpid' .*/
#define WSTOPPED     2		/*Same as WUNTRACED*/
#define WEXITED      4		/* report dead child */
#define WCONTINUED   8		/* report continued child */
#define WNOWAIT      0x01000000 /* Don't reap , just poll status*/

/*Macros for constructing status values */
#define W_EXITCODE(ret,sig)  ((ret) << 8 | (sig) )
#define W_STOPCODE(sig)      ((sig) <<8 | 0x7f)
#define W_CONTINUED	          0xffff
#define WCOREFLAG             0x80

/* IF WIFEXITED(STATUS) ,the low-order 89 bits of the status */
#define WEXITSTATUS(status)   (((status) & 0xff00) >> 8)

/* if WIFSIGNALED(STATUS) , the terminating signal */
#define WTERMSIG(status)      ((status) & 0x7f)

/* if WIFSTOPPED(status)  ,the signal that stopped the child */
#define WSTOPSIG(status)    WEXITSTATUS(status)

/* Nonzero if STATUS indicates normal termination */
#define  WIFEXITED(status)  (WTERMSIG(status)==0)

/* Nonezero if STATUS indicates termination by a signal */
#define WIFSIGNALED(status) \
	(((signed char) (((status) & 0x7f) + 1) >>1) > 0)

/* Nonzero if STATUS indicates the child is stopped */
#define WIFSTOPPED(status) (((status) & 0xff) == 0x7f)

#ifdef WCONTINUED	
#define  WIFCONTINUED(status)  ((status) == W_CONTINUED)	
#endif // WCONTINUED	

#define WCOREDUMP(status)    ((status) & WCOREFLAG)

union wait
{
	int w_status;
	struct  
	{
		unsigned int __w_termsig : 7;   /* terminating signal */
		unsigned int __w_coredump : 1;  /* set if dumped core */
		unsigned int __w_retcode : 8;	/*return code if exitewd normally*/
		unsigned int : 16;
	}__wait_terminated;

	struct
	{
		unsigned int __w_stopval : 8;	/* W_STOPPED if stopped*/
		unsigned int __w_stopsig : 8;	/* Stopping signal */
		unsigned int : 16;
	} __wait_stopped;

};

#define  w_termsig		__wait_terminated.__w_termsig
#define  w_coredump		__wait_terminated.__w_coredump
#define  w_retcode		__wait_terminated.__w_retcode
#define  w_termsig		__wait_terminated.__w_termsig
#define  w_termsig		__wait_terminated.__w_termsig

pid_t waitpid(pid_t a_iPad, int* a_piStatus, int a_iOptions);

#ifdef __cplusplus
}
#endif

#endif

#endif //TWAIT_H