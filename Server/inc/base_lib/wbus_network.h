#ifndef _WBUS_NETWORK_H
#define _WBUS_NETWORK_H

#include "def/type_def.h"
#include "base_lib/queue/ringbuffer.h"
#include "base_lib/queue/async_msg_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
	int32_t tapp_network_init();
	void tapp_netword_uninit();
	int32_t tapp_network_run();
	void network_loop();
	WBUSADDR get_local_addr();
	uint16_t get_local_port();
	CRingBuff* get_peer_buf();
	CRingBuff* get_channer_buf();
	int32_t set_time_stamp(NETTIMERSTAP& time_stamp, int process_usec = 0);

	CHANNELINFO* find_channel(CHANNELID channel_id);
	CHANNELINFO* find_dst_channel(WBUSADDR src_id, WBUSADDR dst_id, BOOL with_recv_data);
	int32_t get_channel_addr(WBUSADDR* out_addrs,int& out_cnt,int svr_type=-1);
	int32_t channel_send(CHANNELID channel_id, char* buf, size_t len);
	int32_t channel_sendv(CHANNELID channel_id, const struct iovec* a_ptVector, const int a_iVecCnt);

	// ! peer --gateway
	PEERINFO* find_peer(PEERID peer_id);
	int32_t peer_send(PEERID peer_id, char* buf, size_t len);
	int32_t peer_sendv(PEERID peer_id, const struct iovec* a_ptVector, const int a_iVecCnt);

	int32_t new_connect_net_addr(CHANNELID channel_id);

	//!to been del
	int32_t new_connect(char* ip, int port);

	int32_t on_channel_connected(CHANNELINFO* info);
	int32_t on_channel_closed(CHANNELINFO* info);

	//¡Ÿ ±ÃÌº”  to do seb
	uint32_t round_up_2power(int32_t size);
	int64_t cur_time();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_WBUS_NETWORK_H









