#ifndef _TIMER_DEF_H_
#define _TIMER_DEF_H_

#define REPAT_FOREVER          (0)
#define TICK_PER_SECOND        (50LL)
#define MS_PER_TICK            (1000/TICK_PER_SECOND)
#define TIMER_MS2TICK(ms)      (((int64_t)(ms))/MS_PER_TICK) //毫秒转tick
#define MAKE_TIMER_ID(owner_id,timer_type) \
		(((uint64_t)owner_id<<32) | (timer_type))
#define MAX_TIMER_ID           (512) //支持自定义timer种类的最大数目
#define MAX_TIMER_CB_DATA_LEN  (64)  //timer实际中回调函数的最大长度

#define TVN_BITS (6)
#define TVR_BITS (8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

#define TV1_MAX (TVR_SIZE)
#define TV2_MAX (1 << (TVR_BITS + 1 * TVN_BITS))
#define TV3_MAX (1 << (TVR_BITS + 2 * TVN_BITS))
#define TV4_MAX (1 << (TVR_BITS + 3 * TVN_BITS))

#define TV1_IDX(tick)  ((tick) & TVR_MASK)
#define TV2_IDX(tick)  (((tick) >> TVR_BITS) & TVN_MASK)
#define TV3_IDX(tick)  (((tick) >> (TVR_BITS + 1 * TVN_BITS))& TVN_MASK)
#define TV4_IDX(tick)  (((tick) >> (TVR_BITS + 2 * TVN_BITS))& TVN_MASK)
#define TV5_IDX(tick)  (((tick) >> (TVR_BITS + 3 * TVN_BITS))& TVN_MASK)

enum E_TIMER_TYPE
{
#define DEFINE_TIMER(type) ttTimer##type,
#include "timer_type.inl"
#undef DEFINE_TIMER
};

extern const char* get_timer_type_name(int32_t type);

#endif //_TIMER_DEF_H_

































