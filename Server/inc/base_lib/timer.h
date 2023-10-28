#ifndef _TIMER_H_
#define _TIMER_H_

#include "define/timer_def.h"

#include "base_lib/id_object_pool.h"
#include "base_lib/shm_object.h"
#include "base_lib/linked_list.h"
#include "base_lib/process_error.h"

class CTimerMgr;

struct CTimerNode
{
	uint64_t qwTimeID;
	int32_t  nTimerType;    //定时器类型
	int64_t  expires;       //超时时间点(tick)
	int32_t  interval;      //间隔时间(tick)
	int64_t  llListOffset;  //当前timer 对象所在list在time_mgr中的偏移
	int32_t  nRemainCount;  //剩余执行次数
	int32_t  nTickCount;    //执行次数
	BOOL     bIsForever;    //永久执行
	BOOL     bIsRunning;    //是否正在被处理，防止超时处理中删除自己

	uint64_t qwOwnerID;
	int32_t  nDataLen;
	char     szData[MAX_TIMER_CB_DATA_LEN]; //数据空间

	LINKED_LIST_NODE stListNode;
};

typedef void(*TIMEOUT_FUNC)(CTimerMgr& rTimerMgr,const CTimerNode& rTimer, void* pCbData, int32_t nDataLen);

struct TIMER_VECTOR
{
	CLinkedList vec[TVN_SIZE];
};

struct TIMER_VECTOR_ROOT
{
	CLinkedList vec[TVR_SIZE];
};


struct STimerData
{
	void init();
	void uninit();

	uint64_t _qwStartTimerTick;
	uint64_t _qwCurTick;
	uint64_t _qwStartTime;
	uint64_t _qwCurTime;
	uint64_t _qwIDGenerator;

	TIMER_VECTOR_ROOT _tv1;
	TIMER_VECTOR _tv2;
	TIMER_VECTOR _tv3;
	TIMER_VECTOR _tv4;
	TIMER_VECTOR _tv5;
};

class CTimerMgr
{
public:
	CTimerMgr();
	~CTimerMgr();

	BOOL init(const char* pcszInstanceTag, int32_t nInitCount, uint64_t qwStartMS, uint64_t qwStartTime);
	BOOL restore(const char* pcszInstanceTag);
	BOOL uninit();
	void mainloop(uint64_t qwCurMS, uint64_t qwCurTime);

	uint64_t add_timer(int32_t timer_type
		, int32_t first_interval    //ms
		, int32_t repeat_interval   //ms
		, int32_t repeat_num
		, const void* pCbData
		, int32_t nCbDataLen
		, uint64_t qwOwnerID = 0);

	uint64_t add_single_timer(int32_t timer_type
		, int32_t interval           //ms
	    , const void* pCbData
	    , int32_t nCbDataLen
	    , uint64_t qwOwnerID = 0);
	
	CTimerNode* find_timer(uint64_t qwTimerID);
	BOOL del_timer(CTimerNode* pTimer);

	uint64_t current_tick() { return _timer_data()->_qwCurTick; };
	uint64_t current_time() { return _timer_data()->_qwCurTime; };
	uint32_t get_tick_time(uint64_t time_tick);

	BOOL set_timer_callback(int32_t nTimerType, TIMEOUT_FUNC func);

	void print(const char* timer_name);
	void print(CTimerNode* pNode);
protected:
	virtual BOOL check_timer_add(CTimerNode* pNode);
	virtual void on_timer_add(CTimerNode* pNode);
	virtual void on_timer_del(CTimerNode* pNode);
	virtual BOOL on_timer_trigger(CTimerNode* pNode);

	template<class TFunc>
	inline BOOL traverse_timer(TFunc& rFunc);

private:
	void add_timer_node(CTimerNode* pNode);
	BOOL del_timer_node(CTimerNode* pNode);
	CTimerNode* get_timer_node(uint64_t qwTimerID);

	CLinkedList* find_handle_list(int64_t expires);
	void cascade(CLinkedList* src);
	void internal_run_timer();

	void set_tick_count(uint64_t qwTickCount);
	void add_tick_count();

	STimerData* _timer_data() { return m_TimerData.get_data(); };
	
private:
	TIMEOUT_FUNC _timeout_funcs[MAX_TIMER_ID];
	CShmObject<STimerData> m_TimerData;
	CIDObjectPool<CTimerNode> m_TimerPool;
};

template<class TFunc>
inline BOOL CTimerMgr::traverse_timer(TFunc& rFunc)
{
	return m_TimerPool.traverse(rFunc);
}

#endif //_TIMER_H_


















