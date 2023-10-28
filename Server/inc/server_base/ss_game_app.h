#ifndef _H_SS_GAMEAPP_H__
#define _H_SS_GAMEAPP_H__

#include <map>
#include "app/tapp.h"

class CGameLog;
class CGameCfg;
class CRedis;
template<typename T> class CDdataManager;
class CGlobalDataManager;

#define gAPP CSsGameApp::get_app()

class CSsGameApp
{
	static CSsGameApp* gApp;

public:
	static CSsGameApp& get_app();
	static void on_server_end_service(int32_t nServiceType,int32_t	nServerAddr,int32_t nParam1,int32_t nParam2);
public:
	CSsGameApp();
	virtual ~CSsGameApp();

public:
	BOOL init();
	void Destroy();
	void Run();
	virtual const char* GetName() const = 0;
	virtual void GetWin(int& x, int& y);
	virtual CRedis* GetDb();

public:
	inline TAPPCTX& get_ctx();
	inline int get_id() const;
	inline const char* get_bus_id() const;
	inline int get_local_addr() { return get_id(); }
	int32_t get_area_id() { return GET_SERVER_AREA(get_id()); }
	int32_t get_type() { return GET_SERVER_TYPE(get_id()); }
	int32_t get_index() { return GET_SERVER_INDEX(get_id()); }
	int64_t read_diff_time();
	void write_diff_time(int64_t& llTimeDiff);
	inline  int GetTick() const;

	CGameLog& GetLog();
	CGameCfg& GetCfg();
	template<typename T> CDdataManager<T>& GetData();

	inline int GetTimeZone() const;
	inline uint64_t GetCurTimestamp() const;
	inline time_t GetCurTime() const;
	inline time_t GetRealTime() const;
	inline struct tm GetCurLocalTime() const;
	inline uint64_t GetStartTick() const;
	inline uint64_t GetCurTick() const;
	inline uint64_t GetRealTick() const;

	inline int64_t GetCurFrame() const;
	inline int32_t GetState() const;
	inline void SetState(int32_t val);
	inline uint64_t GetStopTimer(uint64_t val);
	inline void SetStopTimer(uint64_t val);
	inline int64_t GetTimeDiff() const;
	inline virtual void SetTimeDiff(int64_t val);
	inline void SetStopFlag(BOOL bStopFlag);
	inline void SetTimeDatModifyTime(int64_t val);
	inline int64_t GetTimeDatModifyTime() const;

private:
	inline void set_time_zone(int val);
	inline void set_read_time(time_t val);
	inline void set_real_timestamp(uint64_t val);
	inline void set_cur_local_time(const struct tm& val);
	inline void set_start_tick(uint64_t val);
	inline void set_real_tick(uint64_t val);
	inline void set_cur_frame(int64_t val);
	void _proc_console();
	void _print_db_stat();

private:
	struct STAT
	{
		struct BUS_STAT
		{
			int32_t nInFlux;
			int32_t nOutFlux;
			int32_t nInNum;
			int32_t nOutNum;

			BUS_STAT()
				:nInFlux(0),nOutFlux(0),nInNum(0),nOutNum(0)
			{

			}
		};
		std::map<int32_t ,BUS_STAT> Buss;
	}m_Stat;

	STAT::BUS_STAT* _bus_stat(int32_t nPeerAddr);
	void _cal_bus_stat();
	void _print_bus_stat();

protected:
	virtual BOOL OnIniting(BOOL bResume);
	virtual BOOL OnInit(BOOL bResume);
	virtual BOOL OnInitFail();
	virtual BOOL OnDestroy();
	virtual BOOL OnDestroyed();
	virtual BOOL OnQuit();
	virtual BOOL OnFini();
	virtual void OnProc();
	virtual BOOL OnTick();
	virtual void OnFrame(int64_t frame);
	virtual void OnTimer1();
	virtual void OnTimer10();
	virtual void OnTimer60();
	virtual void OnTimer300();
	virtual BOOL OnReload();
	virtual BOOL OnConnect(const LINKBASE* info);
	virtual BOOL OnDisConnect(const LINKBASE* info);
	virtual BOOL OnConnectAll();

private:
	static int __OnInit(TAPPCTX* ctx, void* args);
	static int __OnDestroy(TAPPCTX* ctx, void* args);
	static int __OnQuit(TAPPCTX* ctx, void* args);
	static int __OnFini(TAPPCTX* ctx, void* args);
	static int __OnProc(TAPPCTX* ctx, void* args);
	static int __OnTick(TAPPCTX* ctx, void* args);
	static int __OnReload(TAPPCTX* ctx, void* args);
	static int __OnConnect(TAPPCTX* ctx, void* args, const LINKBASE* info);
	static int __OnDisconnect(TAPPCTX* ctx, void* args, const LINKBASE* info);

	BOOL _OnInit();
	BOOL _OnDestroy();
	BOOL _OnQuit();
	BOOL _OnFini();
	BOOL _OnProc();
	BOOL _OnTick();
	BOOL _OnReload();
	BOOL _OnConnect(const LINKBASE* info);
	BOOL _OnDisconnect(const LINKBASE* info);

protected:
	TAPPCTX m_Ctx;
	BOOL m_Quit;
	CGameLog* m_Log;
	CGameCfg* m_Cfg;
	int m_TimeZone;
	time_t m_RealTime;
	uint64_t m_RealTimestamp;
	struct tm m_CurLocalTime;
	uint64_t m_StartTick;
	uint64_t m_RealTick;
	int64_t m_CurFrame;
	int32_t m_State;
	uint64_t m_StopTimer;
	int64_t  m_llTimeDiff;
	BOOL     m_bStopFlag;
	int64_t  m_llTimeDatModifyTime;


};


#endif// _H_SS_GAMEAPP_H__