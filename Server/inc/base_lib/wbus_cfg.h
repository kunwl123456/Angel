#ifndef _WBUS_CFG_H_
#define _WBUS_CFG_H_

#include "wbus_def.h"
#include "wbus_error.h"
#include "define/server_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

	int32_t init_wbus_cfg(const char* name, BOOL resume, AGBUSLOCALINFO& info);
	void uninit_wbus_cfg();

	int32_t get_wbus_cfg_addrs(WBUSADDR* (&addrs), int32_t& count);
	AGBUSPAIRNETADDR* get_cfg_addr_by_channel(const uint64_t channel_id);

	BOOL is_connected_all();
	int32_t reconnect_by_cfg();

	int32_t on_cfg_channel_closed(const CHANNELID& channel_id);
	int32_t on_cfg_channel_connected(const CHANNELID& channel_id);


#ifdef __cplusplus
}
#endif



#endif //_WBUS_CFG_H_