#include "stdafx.h"
#include "timer.h"

#include "base_lib/hash.h"
#include "base_lib/constant.h"
#include "base_lib/core_dumper.h"

#include "define/timer_def.h"


void STimerData::init()
{
	_qwStartTimerTick = 0;
	_qwCurTick = 0;
	_qwStartTime = 0;
	_qwCurTime = 0;
	_qwIDGenerator = 0;

	for (int32_t i = 0; i < ARR_LEN(_tv1.vec); ++i)
		_tv1.vec[i].init();

	for (int32_t i = 0; i < ARR_LEN(_tv2.vec); ++i)
		_tv2.vec[i].init();

	for (int32_t i = 0; i < ARR_LEN(_tv3.vec); ++i)
		_tv3.vec[i].init();

	for (int32_t i = 0; i < ARR_LEN(_tv4.vec); ++i)
		_tv4.vec[i].init();

	for (int32_t i = 0; i < ARR_LEN(_tv5.vec); ++i)
		_tv5.vec[i].init();


	return;
}

void STimerData::uninit()
{
	for (int32_t i = 0; i < ARR_LEN(_tv1.vec); ++i)
		_tv1.vec[i].uninit();

	for (int32_t i = 0; i < ARR_LEN(_tv2.vec); ++i)
		_tv2.vec[i].uninit();

	for (int32_t i = 0; i < ARR_LEN(_tv3.vec); ++i)
		_tv3.vec[i].uninit();

	for (int32_t i = 0; i < ARR_LEN(_tv4.vec); ++i)
		_tv4.vec[i].uninit();

	for (int32_t i = 0; i < ARR_LEN(_tv5.vec); ++i)
		_tv5.vec[i].uninit();

}

CTimerMgr::CTimerMgr()
{
	memset(_timeout_funcs, 0, sizeof(_timeout_funcs));
}

CTimerMgr::~CTimerMgr()
{

}

BOOL CTimerMgr::init(const char* pcszInstanceTag, int32_t nInitCount, uint64_t qwStartMS, uint64_t qwStartTime)
{
	int32_t nRetCode = 0;

	nRetCode = m_TimerData.init(pcszInstanceTag);
	if (!nRetCode) return FALSE;
	if (!m_TimerData.create_data()) return FALSE;

	nRetCode = m_TimerPool.init(nInitCount, pcszInstanceTag);
	if (!nRetCode) return FALSE;
	if (!_timer_data()) return FALSE;

	_timer_data()->init();
	_timer_data()->_qwCurTick = TIMER_MS2TICK(qwStartMS);
	_timer_data()->_qwStartTimerTick = current_tick();

	_timer_data()->_qwCurTime = qwStartTime;
	_timer_data()->_qwStartTime = current_time();

	//INF("timer init start_tick:%;;d,start_time = %lld",current_tick(),current_time());

	return TRUE;
Exit0:
	return FALSE;
}


BOOL CTimerMgr::restore(const char* pcszInstanceTag)
{
	int32_t nRetCode = 0;

	nRetCode = m_TimerData.init(pcszInstanceTag);
	LOG_PROCESS_ERROR(nRetCode);

	LOG_PROCESS_ERROR(m_TimerData.restore_data());

	nRetCode = m_TimerPool.restore(pcszInstanceTag);
	LOG_PROCESS_ERROR(nRetCode);
	LOG_PROCESS_ERROR(_timer_data());

	//INF("timer restore start_tick:%lld");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CTimerMgr::uninit()
{
	_timer_data()->uninit();

	int32_t nRetCode = 0;
	nRetCode = m_TimerData.uninit();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_TimerPool.uninit();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

void CTimerMgr::mainloop(uint64_t qwCurMS, uint64_t qwCurTime)
{
	uint64_t qwCurTick = TIMER_MS2TICK(qwCurMS);
	uint64_t qwTickNum = qwCurTick - this->current_tick();

	if (qwCurTick < this->current_tick())
	{
		//CRI("time is go back old tick:%llu ,new tick:%lld",current_tick(),qwCurTick);
		//time is go back ,reset time
		set_tick_count(qwCurTick);
		qwTickNum = 0;
	}

	for (int64_t i = 0; i < qwTickNum; ++i)
	{
		internal_run_timer();
		add_tick_count();
	}

	if (qwCurTick != current_tick()) return;
	_timer_data()->_qwCurTime = qwCurTime;

	return;
}

uint64_t CTimerMgr::add_timer(int32_t timer_type
	, int32_t first_interval_ms    //ms
	, int32_t repeat_interval_ms   //ms
	, int32_t repeat_num
	, const void* pCbData
	, int32_t nCbDataLen
	, uint64_t qwOwnerID )
{
	first_interval_ms = angle_max((int32_t)MS_PER_TICK, first_interval_ms);

	int32_t nRetCode = 0;
	CTimerNode* pTimer = NULL;
	uint32_t first_timer_tick = TIMER_MS2TICK(first_interval_ms);
	uint32_t repeated_timer_tick = TIMER_MS2TICK(repeat_interval_ms);
	uint64_t qwTimerID = ++_timer_data()->_qwIDGenerator;
	CLinkedList* list = NULL;

	LOG_PROCESS_ERROR(nCbDataLen < MAX_TIMER_CB_DATA_LEN);
	LOG_PROCESS_ERROR(timer_type > 0);
	LOG_PROCESS_ERROR(qwTimerID > 0);

	pTimer = m_TimerPool.new_object(qwTimerID);
	LOG_PROCESS_ERROR(pTimer);

	pTimer->qwTimeID = qwTimerID;
	pTimer->expires = current_tick() + first_interval_ms;
	pTimer->interval = repeated_timer_tick;
	pTimer->nTimerType = timer_type;
	pTimer->nRemainCount = repeat_num;
	pTimer->nTickCount = 0;
	pTimer->bIsForever = (repeat_num == REPAT_FOREVER);
	pTimer->bIsRunning = 0;
	pTimer->qwOwnerID = qwOwnerID;
	pTimer->nDataLen = nCbDataLen;

	if (nCbDataLen > 0)
	{
		memcpy(pTimer->szData, pCbData, nCbDataLen);
	}

	nRetCode = check_timer_add(pTimer);
	LOG_PROCESS_ERROR(nRetCode);

	add_timer_node(pTimer);

	return qwTimerID;
Exit0:
	return 0;
}

uint64_t CTimerMgr::add_single_timer(int32_t timer_type
	, int32_t interval           //ms
	, const void* pCbData
	, int32_t nCbDataLen
	, uint64_t qwOwnerID)
{
	return add_timer(timer_type, interval, 0, 1, pCbData, nCbDataLen, qwOwnerID);
}

void CTimerMgr::add_timer_node(CTimerNode* pNode)
{
	int32_t nRetCode = 0;
	CLinkedList* list = find_handle_list(pNode->expires);
	pNode->llListOffset = (int64_t)list - (int64_t)&_timer_data()->_tv1;
	nRetCode = list->push_rear(&pNode->stListNode);
	if (!nRetCode) return;

	on_timer_add(pNode);

	return;
}

BOOL CTimerMgr::del_timer_node(CTimerNode* pNode)
{
	CLinkedList* list = NULL;
	int32_t nRetCode = 0;
	if (!pNode) return FALSE;
	if (pNode->llListOffset < 0) return FALSE;  //偏移过小
	if(pNode->llListOffset >= (int)sizeof(*_timer_data())) return FALSE; //偏移过大

	list = (CLinkedList*)((int64_t)&(_timer_data()->_tv1) + pNode->llListOffset);
	nRetCode = list->remove(&pNode->stListNode);
	if (!nRetCode) return FALSE;

	on_timer_del(pNode);

	nRetCode = m_TimerPool.delete_object(pNode);
	if (!nRetCode) return FALSE;

	return TRUE;
Exit0:
	return FALSE;
}

CTimerNode* CTimerMgr::get_timer_node(uint64_t qwTimerID)
{
	if (qwTimerID == 0)
	{
		return NULL;
	}

	return m_TimerPool.find(qwTimerID);
}

//（时间轮算法）根据新加timer的超时时间找到合适的链表挂上去（先进先出FIFO）
CLinkedList* CTimerMgr::find_handle_list(int64_t expires)
{
	//idx其实就是过期时间和当前tick的
	uint64_t idx = expires - this->current_tick();

	if (idx < TV1_MAX)
	{
		return &_timer_data()->_tv1.vec[TV1_IDX(expires)];
	}
	else if (idx < TV2_MAX)
	{
		return &_timer_data()->_tv2.vec[TV2_IDX(expires)];
	}
	else if (idx < TV3_MAX)
	{
		return &_timer_data()->_tv3.vec[TV3_IDX(expires)];
	}
	else if (idx < TV4_MAX)
	{
		return &_timer_data()->_tv4.vec[TV4_IDX(expires)];
	}
	else
	{
		/*
			如果超时时间大于64位的0xffffffff，那就放大第五层时间轮 
		*/

		if (idx > 0xffffffffUL)
		{
			idx = 0xffffffffUL;
			expires = idx + this->current_tick();
		}
		return &_timer_data()->_tv5.vec[TV5_IDX(expires)];
	}



}

//根据当前服务器运行到的事件将src链表中的所有timer实例重新挂入新的列表
void CTimerMgr::cascade(CLinkedList* src)
{
	int32_t nRetCode = 0;
	LINKED_LIST_NODE* pNode = NULL;

	//链表节点从头拿出来
	while (pNode = src->pop_head())
	{
		//根据数据成员拿到这个对象的地址
		CTimerNode* pTimer = list_node_cast(CTimerNode*, stListNode, pNode);

		//根据过期事件找到对应的列表
		CLinkedList* dst = find_handle_list(pTimer->expires);
		//
		pTimer->llListOffset = (int64_t)dst - (int64_t)&(_timer_data()->_tv1);

		nRetCode = dst->push_rear(pNode);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return;
Exit0:
	return;
}

void CTimerMgr::internal_run_timer()
{
	int tv1_idx = TV1_IDX(this->current_tick());
	int32_t nRetCode = 0;

	do
	{
		//表示在tv1内
		if (tv1_idx != 0) break;

		int tv2_idx = TV2_IDX(this->current_tick());
		cascade(&_timer_data()->_tv2.vec[tv2_idx]);
		if (tv2_idx != 0) break;

		int tv3_idx = TV3_IDX(this->current_tick());
		cascade(&_timer_data()->_tv3.vec[tv3_idx]);
		if (tv3_idx != 0) break;

		int tv4_idx = TV4_IDX(this->current_tick());
		cascade(&_timer_data()->_tv4.vec[tv4_idx]);
		if (tv4_idx != 0) break;

		int tv5_idx = TV2_IDX(this->current_tick());
		cascade(&_timer_data()->_tv5.vec[tv5_idx]);
		if (tv5_idx != 0) break;

	} while (0);

	CLinkedList* list = &_timer_data()->_tv1.vec[tv1_idx];
	LINKED_LIST_NODE* pNode = list->head().pNext;

	while (pNode != &list->rear())
	{
		CTimerNode* pTimer = list_node_cast(CTimerNode*, stListNode, pNode);
		LINKED_LIST_NODE* pNext = pNode->pNext;
		
		//若扫到一个正在running状态的timer，说明是上次callback阶段宕机了，在resume阶段又被扫到
		//此时应该将其移除
		if (pTimer->bIsRunning)
		{
			//CRI("打印各类参数");
			if (pTimer->nRemainCount <= 0 && !pTimer->bIsForever)
			{
				del_timer_node(pTimer);
				pNode = pNext;
				continue;
			}
		}

		pTimer->bIsRunning = 1;
		pTimer->nRemainCount--;
		pTimer->nTickCount++;

		CORE_DUMPER_BEGIN
		{
			if (this->_timeout_funcs[pTimer->nTimerType]);
			{
				(this->_timeout_funcs[pTimer->nTimerType])(*this, *pTimer, (void*)pTimer->szData, pTimer->nDataLen);
				this->on_timer_trigger(pTimer);
			}
		}
		CORE_DUMPER_PROCESS
		{
			//运行期间dump，删除定时器
			//CRI
			del_timer(pTimer);
		}
			CORE_DUMPER_END

		pTimer->bIsRunning = 0;

		if ((pTimer->nRemainCount <= 0) && !pTimer->bIsForever)
		{
			del_timer_node(pTimer);
			pNode = pNext;
			continue;
		}

		//没过期，更新间隔并扔到新的队列，更新位移
		pTimer->expires = current_tick() + pTimer->interval;
		CLinkedList* new_list = find_handle_list(pTimer->expires);
		pTimer->llListOffset = (int64_t)new_list - (int64_t)&(_timer_data()->_tv1);

		nRetCode = list->remove(pNode);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = new_list->push_rear(pNode);
		LOG_PROCESS_ERROR(nRetCode);

		pNode = pNext;
	}

	return;
Exit0:
	return;
}

void CTimerMgr::set_tick_count(uint64_t qwTickCount)
{
	_timer_data()->_qwCurTick = qwTickCount;
}

void CTimerMgr::add_tick_count()
{
	_timer_data()->_qwCurTick++;
}

CTimerNode* CTimerMgr::find_timer(uint64_t qwTimerID)
{
	return get_timer_node(qwTimerID);
}

BOOL CTimerMgr::del_timer(CTimerNode* pNode)
{
	int32_t nRetCode = 0;
	LOG_PROCESS_ERROR(pNode);

	if (pNode->bIsRunning)
	{
		pNode->nRemainCount = 0;
		pNode->bIsForever = 0;
		return TRUE;
	}

	//实际去删除定时器节点在链表
	nRetCode = del_timer_node(pNode);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

uint32_t CTimerMgr::get_tick_time(uint64_t time_tick)
{
	LOG_PROCESS_ERROR(time_tick >= _timer_data()->_qwStartTimerTick);

	//有疑问？？？
	return _timer_data()->_qwStartTime + (time_tick - _timer_data()->_qwStartTimerTick) / TICK_PER_SECOND;

Exit0:
	return 0;
}

BOOL CTimerMgr::set_timer_callback(int32_t nTimerType, TIMEOUT_FUNC func)
{
	if (nTimerType >= MAX_TIMER_ID) return FALSE;

	if (nTimerType <= ttTimerInvalid || nTimerType > MAX_TIMER_ID)
		return FALSE;
	if (!func) return FALSE;

	this->_timeout_funcs[nTimerType] = func;

	return TRUE;
Exit0:
	return FALSE;
}

struct TRAVERSE_TIMER
{
	uint32_t    timer_id;
	CTimerMgr*  _pTimerMgr;

	BOOL operator()(uint64_t qwTimerID, CTimerNode* pTimer)
	{
		if (timer_id == 0 || pTimer->nTimerType == timer_id)
		{
			_pTimerMgr->print(pTimer);
		}
		return TRUE;
	}

};

void CTimerMgr::print(CTimerNode* pTimer)
{
	if (!pTimer) return;

	//打印时间节点的各项数据
}

BOOL check_timer_add(CTimerNode* pNode)
{
	return TRUE;
}

const char* get_timer_type_name(int32_t type)
{
	switch (E_TIMER_TYPE(type))
	{
#define DEFINE_TIMER(type) case ttTimer##type: return #type;
#include "define/timer_type.inl"
#undef  DEFINE_TIMER
	}

	return "unknown";
}








