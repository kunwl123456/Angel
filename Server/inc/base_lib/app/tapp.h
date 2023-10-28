#ifndef TAPP_H
#define TAPP_H

#include "def/type_def.h"
#include "app/tapp_error.h"

#define TAPP_MAKE_VERSION(major,minor,rev,build) (((unsigned long long)(major))<<56 | ((unsigned long long)(minor))<<48 | ((unsigned long long)(rev))<<32 | (build))
#define TAPP_GET_MAJOR(ver)   ((ver)>>56 & 0xff)
#define TAPP_GET_MINOR(ver)   ((ver)>>48 & 0xff)
#define TAPP_GET_REV(ver)     ((ver)>>32 & 0xff)
#define TAPP_GET_BUILD(ver)   ((ver) & 0xffffffff)

#define TAPP_START_MODE_INIT   0
#define TAPP_START_MODE_RESUME 1

#ifdef __cplusplus
extern "C"
{
#endif
	#define pthread_t unsigned int
	struct tagTappData
	{
		//the buffer for user to write data, can be casted to user structure
		char*    pszBuff;
		
		//the length of buffer ,you should use assertion here
		size_t   iLen;
		
		//the meta name of tapp data
		char*    pszMetaName;
		
		//the pointer to the meta
		intptr_t iMeta;
		void*    iLib;
	};

	typedef struct tagTappData TAPPDATA;
	typedef struct tagTappData *LPTAPPDATA;

	struct  tagTAPPCTX;
	struct  tagTappThreadConf;

	struct  tagTappThreadCtx;
	typedef struct tagTappThreadCtx TAPPTHREADCTX;
	typedef struct tagTappThreadCtx *LPTAPPTHREADCTX;

	//The common thread config files
	struct tagTappThreadConf
	{
		//called first after thread was created
		int (*pfnInit)(IN TAPPTHREADCTX* pstThreadCtx, IN void* pvArg);

		//like tapp proc ,called everytime in the proc
		int (*pfnProc)(IN TAPPTHREADCTX* pstThreadCtx,IN void* pvArg);

		//like tapp tick ,called everytime the tapp tick reaches
		int (*pfnTick)(IN TAPPTHREADCTX* pstThreadCtx, IN void* pvArg);

		//thre reload call back , called when the reload signal received
		int (*pfnReload)(IN TAPPTHREADCTX* pstThreadCtx, IN void* pvArg);

		//called when the thread finis
		int (*pfnFini)(IN TAPPTHREADCTX* pstThreadCtx, IN void* pvArg);

		//if Idle is NULL, thread usleep(1000* pstInnerCtx->iIdleSleep),else call user's pfnIdle
		int (*pfnIdle)(IN TAPPTHREADCTX* pstThreadCtx, IN void* pvArg);

		//the metaname for run data cumu
		char szRunDataCumuMeta[512];

		//metaname for run status data
		char szRunStatusMeta[512];

		//Name of threads
		char szThreadName[128];

		//called when the main thread has detected that the thread has fini
		int(*pfnCleanup)(struct tagTAPPCTX* pstCtx, TAPPTHREADCTX* pstThreadCtx,
			void* pvArgMain, void* pvArgThread);
	};

	typedef struct tagTappThreadConf TAPPTHREADCONF;
	typedef struct tagTappThreadConf *LPTAPPTHREADCONF;

	struct tagTAPPCTX
	{
		//should be set before calling tapp_def_init
		int argc;

		//should be set before calling tapp_def_init
		char** argv;

		//can be set before calling tapp_def_init
		//can be set by user
		unsigned long long uiVersion;

		//the tick timer period
		//can be set before calling tapp_def_init
		//if not,tapp_def_init will set it to a by option
		//if not  ' --timer' ,set it with default value 10ms
		int iTimer;

		//can be set before calling tapp_def_init
		//if not,tapp_def_init will set it to a default value
		//the milliseconds to wait the epoll-wait timeout
		int iEpollWait;

		//can be set before calling tapp_def_init
		//if not,tapp_def_init will set it to a default value
		//when an internal counter reaches iIdleCount ,call pfnIdle of sleep for a while 
		int iIdleCount;

		//can be set before calling tapp_def_init
		//if not ,tapp_def_init will set it to a default value
		int iIdleSleep;
		int iTickCount;

		//bussiness id assigned by target
		int iBusinessID;

		//shm key for tbus GCIM
		char* pszGCIMKey;

		//!0 :use tbus; 0:not use tbus
		int iUseBus;

		//iBus is really handle of tbus
		int iBus;

		//identifier of process , if tbus is used, record tbus-address
		const char* pszId;

		//bus id
		int iId;

		unsigned long long vm_space_size;
		
		//read-only
		struct timeval stCurr;

		//read-only
		const char* pszApp;
		const char* pszConfFile;

		//tlog cfg file path
		const char* pszLogConfFile;
		const char* pszMBaseKey;
		const char* pszTsmIdxKey;

		//if used,and non-zero ,can be cast to TDRMETALIB* pointer
		intptr_t iLib;

		//if set,tapp will not automatically load conf-file
		int iNoLoadConf;

		TAPPDATA stConfData;        //conf data
		TAPPDATA stConfPrepareData; //when reload,can use this for prepare load conf

		TAPPDATA stRunDataStatus;   //run time data for status val
		TAPPDATA stRunDataCumu;	    //run time data for cumulate val

		//number of thread to start , we will just create Nthreads slot for threads
		int iNthreads;

		//config for default thread to start with
		TAPPTHREADCONF stThreadConf;

		//the channel size for tbus,default 1M can be changed by cmdline options
		int iChannelSize;

		//save tapp start mode ,defaultLTAPP_START_MODE_INIT
		int iStartMode;

		//save tapp start mode ,default :TAPP_START_MODE_INIT
		int iNotOutputStat;

		//bus channel timeout gap
		int iBusTimeOutGap;

		//bus use saved time or use gettimeofday
		int iBusUseFreshTime;

		//time interval for bus  channel to send heartbeat msg
		int iBusHeartBeatGap;

		//time interval to check bus channel peer state
		int iBuscheckPeerGap;

		//format of config-file,refer to enum "tagTDRXmlDataFormat" defined in tdr_define.h
		int iConfigFileFormat;

		//the metalib_id in monitor plus system,used for data reporting
		int iReportMetalibId;

		//the dir addr used for data reporting, the format is tcp
		const char* pszReportDirAddrs;

		//the interval for data reporting second
		int iReportInterval;

		//whether to disable cumu data reporting 
		int iDisableCumuReport;

		//whether to disable status data reporting
		int iDisableStausReport;

		int (*pfnArgv)(struct tagTAPPCTX* pstCtx, void* pvArg);
		int (*pfnInit)(struct tagTAPPCTX* pstCtx, void* pvArg);
		int (*pfnProc)(struct tagTAPPCTX* pstCtx, void* pvArg);
		int (*pfnTick)(struct tagTAPPCTX* pstCtx, void* pvArg);
		int (*pfnReload)(struct tagTAPPCTX* pstCtx, void* pvArg);
		int (*pfnIdle)(struct tagTAPPCTX* pstCtx, void* pvArg);

		//when received SIGTERM,call this function ,
		//if -1 is returned ,the mainloop will break
		int (*pfnQuit)(struct tagTAPPCTX* pstCtx, void* pvArg);

		//when received SIGUSR1,call this function ,
		//if -1 is returned ,the mainloop will break
		int (*pfnStop)(struct tagTAPPCTX* pstCtx, void* pvArg);

		int (*pfnFini)(struct tagTAPPCTX* pstCtx, void* pvArg);

		int (*pfnUsage)(int argc, char* argv[]);

		int (*pfnConnect)(struct tagTAPPCTX* pstCtx, void* pvArg,const LINKBASE* info);

		int (*pfnDisconnect)(struct tagTAPPCTX* pstCtx, void* pvArg, const LINKBASE* info);


	};

	typedef struct tagTAPPCTX TAPPCTX;
	typedef struct tagTAPPCTX *LPTAPPCTX;

#if !defined(_WIN32) && !defined (_WIN64)

#endif

#ifdef __cplusplus
}
#endif

#endif //TAPP_H