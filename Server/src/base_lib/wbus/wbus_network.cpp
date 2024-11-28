#include "stdafx.h"
//#include <define/string_def.h>
#include "base_lib/queue/async_msg_queue.h"
#include "base_lib//queue/ringbuffer.h"
#include <event.h>
#include <event2/util.h>
#include <event2/listener.h>
//#include <tinyxml.h>

#include "base_lib/wbus.h"
#include "base_lib/wbus_def.h"
#include "wbus_cfg.h"
#include "wbus_network.h"
#include "app/tapp.h"
//#include "base_lib/container/fixed_map.h"
#include "base_lib/id_object_pool.h"


#include "base_lib/error_code.h"
#include "process_error.h"


struct tag_Network_timer
{
	int64_t interval_time;
	int64_t next_time;
	int32_t remain_times;
};

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/
	extern TAPPCTX* g_app_ctx;
	extern void* g_app_arg;
	extern AGBUSLOCALINFO g_local_info;
	static struct event_base* g_event_base = NULL;
	static struct evconnlistener* g_event_listener[2] = { NULL,NULL };
	static struct tag_Network_timer g_reconnect_timer;
	static CIDObjectPool<CHANNELINFO> g_channel_info_pool;
	//static sk::fixed_map<int32_t, CHANNELINFO*, MAX_CHANNEL_CNT> g_channel_info_map;
	static std::map<int32_t, CHANNELINFO*> g_channel_info_map;
	static CIDObjectPool<PEERINFO> g_peer_info_pool;
	static CRingBuff* g_peer_recv_buff = NULL;
	static CRingBuff* g_channel_recv_buff = NULL;

	static void conn_writecb(struct bufferevent* bev, void* user_data);
	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);

	static void listener_channel_cb(struct evconnlistener* listener, 
		evutil_socket_t fd, struct sockaddr* sa, int socklen, void*user_data);
	static void listener_peer_cb(struct evconnlistener* listener,
		evutil_socket_t fd,struct sockaddr* sa,int socklen,void* user_data);

	BOOL pop_event(NETWORKEVENT& evt, int type, char* data, int len);
	BOOL push_event(NETWORKEVENT& evt, int type, struct bufferevent* bev);

	static void _close_link(LINKBASE* base, struct bufferevent* bev);
	static int32_t _network_init(TAPPCTX* a_pstCtx);
	static int32_t _network_uninit();
	static void _set_reconnect_timer();
	static CHANNELINFO* _new_channel_info(struct bufferevent* bev,CHANNELID channel_id,BOOL is_active = FALSE);
	static PEERINFO* _new_peer_info(struct bufferevent* bev, int net_ip, PEERID peer_id);
	static void _conn_channel_readcb(struct bufferevent* bev, void* user_data);
	static void _conn_peer_readcb(struct bufferevent* bev, void* user_data);
	static AGBUSBASEHEAD* _add_base_head(const char* buf, int len);
	static char* _del_head(AGBUSBASEHEAD* head);
	static int32_t get_opposite_addr(int addr1, int addr2);
	static int32_t get_opposite_port(int port1, int port2);

	struct peer_find_traverse
	{
		BOOL operator()(uint64_t id, PEERINFO* peer_info)
		{
			if (peer_info->peer_id == peer_id)
			{
				pOut = peer_info;
				return TRUE;
			}
		}
		int peer_id;
		PEERINFO* pOut;
	};

	static int32_t network_data_init(BOOL resume)
	{
		g_channel_info_pool.init(MAX_CHANNEL_CNT, CHANEL_INFO_NAME);
		g_peer_info_pool.init(MAX_PEER_CNT, PEER_INFO_NAME);
		//g_channel_info_map.init();
		g_channel_info_map.clear();
		return errSuccess;
	}

	static int32_t network_data_uninit()
	{
		g_channel_info_pool.uninit();
		g_peer_info_pool.uninit();
		//g_channel_info_map.uninit();
		g_channel_info_map.clear();
		return errSuccess;
	}

	static int32_t get_opposite_addr(int addr1, int addr2)
	{
		return addr1 == get_local_addr() ? addr2 : addr1;
	}

	static int32_t get_opposite_port(int port1, int port2)
	{
		return port1 == get_local_port() ?  port1 : port2;
	}

	int tapp_network_init()
	{
		if (!g_app_ctx) return ANGEL_NULL_POINTER;
		BOOL is_resume = g_app_ctx->iStartMode == TAPP_START_MODE_RESUME;

		network_data_init(is_resume);
		init_wbus_cfg(g_app_ctx->pszConfFile, is_resume, g_local_info);
		_network_init(g_app_ctx);
		return AGERR_SUCCESS;

	}

	void tapp_network_uninit()
	{
		_network_uninit();
		uninit_wbus_cfg();
		network_data_uninit();
	}

	int tapp_network_run()
	{
		network_loop();
		return event_base_loop(g_event_base, EVLOOP_NONBLOCK);
	}

	int set_time_stamp(NETTIMERSTAP& time_stamp, int process_usec /* = 0 */)
	{
		struct timeval newtime;
		//gettimeofday(&newtime, NULL);
		evutil_gettimeofday(&newtime, NULL);
		time_stamp.llSec = newtime.tv_sec;
		time_stamp.dwUSec = newtime.tv_usec;
		time_stamp.dwCumm += process_usec;
		time_stamp.iIsAvailable = false;

		return AGERR_SUCCESS;
	}

	char* _generater_shm_name(int src_virtual_ip, int dst_virtual_ip)
	{
		static char sh_name[POOL_NAME_LEN] = { 0 };
		int src_ip, dst_ip;
		src_ip = src_virtual_ip == get_local_addr() ? get_local_addr() : dst_virtual_ip;
		dst_ip = src_virtual_ip == get_local_addr() ? dst_virtual_ip : get_local_addr();
	
		sprintf(sh_name, "channel_ring_buff_%d_%d", src_ip, dst_ip);
		return sh_name;
	}

	static PEERINFO* _new_peer_info(struct bufferevent* bev, int net_ip, PEERID peer_id)
	{
		PEERINFO* info = g_peer_info_pool.find(peer_id);
		if (info) return info;
		
		info = g_peer_info_pool.new_object(peer_id);
		if (!info)
		{
			bufferevent_free(bev);
			//打印日志
			//error_tlog("listener_cb new channel info failed!");
			return NULL;
		}
		memset(info, 0, sizeof(PEERINFO));
		if (!g_peer_recv_buff)
		{
			uint32_t up_size = round_up_2power(g_local_info.peer_addr.recv_buff_size);
			g_peer_recv_buff = (CRingBuff*)malloc(sizeof(CRingBuff) + up_size);
			g_peer_recv_buff->init(up_size);
		}
		info->base = g_event_base;
		info->bufvents = bev;
		info->peer_id = peer_id;
		info->src_virtual_addr = 0;
		info->src_addr = net_ip;

		info->dst_addr = 0;
		info->dst_virtual_addr = get_local_addr();
		info->recv_buf = g_peer_recv_buff;
		info->type = ANGELBUS_CLIENT_ADDR;

		return info;

	}

	CHANNELINFO* _new_channel_info(struct bufferevent* bev, CHANNELID channel_id, BOOL is_active)
	{
		AGBUSPAIRNETADDR* pair_addr = get_cfg_addr_by_channel(channel_id);
		CHANNELINFO* info = g_channel_info_pool.new_object(channel_id);
		if (!info || !pair_addr)
		{
			int opposite_addr = 0;
			if (pair_addr)
				opposite_addr = get_opposite_addr(pair_addr->bus1.virtual_addr, pair_addr->bus2.virtual_addr);
			bufferevent_free(bev);
			//打印日志
			//error_tlog("listener_cb new channel<%llu> ip<%s>info failed!" ,channel_id,wbus_addr_ntoa(opposite_addr))
			return NULL;
		}

		memset(info, 0, sizeof(CHANNELINFO));

		info->base = g_event_base;
		info->bufvents = bev;
		info->channel_id = channel_id;
		info->src_virtual_addr = pair_addr->bus1.virtual_addr;
		info->src_addr = pair_addr->bus1.net_ip;
		info->src_port = pair_addr->bus1.port;
		info->dst_addr = pair_addr->bus2.net_ip;
		info->dst_virtual_addr = pair_addr->bus2.virtual_addr;
		info->dst_port = pair_addr->bus2.port;
		int opposite_addr = get_opposite_addr(pair_addr->bus1.virtual_addr, pair_addr->bus2.virtual_addr);
		g_channel_info_map[opposite_addr] = info;

		if (!g_channel_recv_buff)
		{
			uint32_t up_size = round_up_2power(MAX_LOCAL_CHANNEL_RECEV_BUFF_SIZE);
			g_channel_recv_buff = create_ringbuff(_generater_shm_name(pair_addr->bus1.virtual_addr,pair_addr->bus2.virtual_addr)
				, up_size + sizeof(CRingBuff));

			assert_retval(g_channel_recv_buff, NULL);
			g_channel_recv_buff->init(up_size);
		}

		info->recv_buf = g_channel_recv_buff;
		info->type = AGBUS_SERVER_ADDR;
		
		//
		//info_tlog("listener_cb new channel info<%s> channel <%llu>", wbus_addr_ntoa(opposite_addr), channel_id);
		return info;
	}

	//!connect return
	void _connect_cb(evutil_socket_t st, short flag, void* user_data)
	{
		int error = 0;
		ev_socklen_t len = sizeof(errno);
		assert_retnone(user_data);

		getsockopt(st, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

		//! 超时还没连接，就断线处理
		if (error != 0)
		{
			_close_link((LINKBASE*)user_data, NULL);
			return;
		}
	}

	void _client_event_cb(struct bufferevent* bev, short what, void* userdata)
	{
		if (what & BEV_EVENT_EOF)
		{
			//info_tlog("connect closed");
		}
		else if (what & BEV_EVENT_ERROR)
		{
			//info_tlog("some other error");
		}
		else if(what & BEV_EVENT_CONNECTED && userdata)
		{
			assert_retnone(((LINKBASE*)userdata)->type == AGBUS_SERVER_ADDR);
			CHANNELINFO* info =(CHANNELINFO*)(userdata);
			NETWORKEVENT net_evt;

			net_evt.cce.port = g_local_info.bus_addr.local_port;
			net_evt.cce.virtual_ip = g_local_info.bus_addr.local_ip;
			push_event(net_evt, NET_CHANNEL_CONNECT, bev);

			on_channel_connected(info);

			//! debug
			WBUSADDR ip = get_opposite_addr(info->src_virtual_addr, info->dst_virtual_addr);
			int port = get_opposite_port(info->src_port, info->dst_port);
			//info_tlog("dst_ip<%s> port<%d> send msg ok",wbus_addr_ntoa(ip),port);
			return;
		}
		assert_retnone(bev && userdata);
		_close_link((LINKBASE*)userdata, bev);
	}

	//!这里不可能有peer
	int32_t new_connect_net_addr(CHANNELID channel_id)
	{
		AGBUSPAIRNETADDR* pair_addr = get_cfg_addr_by_channel(channel_id);
		assert_retval(pair_addr, AGERR_NULL_POINTER);
		//init server info
		struct sockaddr_in serv;
		memset(&serv, 0, sizeof(serv));
		serv.sin_family = AF_INET;
		serv.sin_port = htons(pair_addr->bus2.port);
		serv.sin_addr.s_addr = pair_addr->bus2.net_ip;

		evutil_socket_t fd = -1;
		//通信的fd放到bufferevent中
		struct bufferevent* bev = NULL;
		assert_retval(g_event_base, AGERR_INVALID_PARAM);
		bev = bufferevent_socket_new(g_event_base, fd, BEV_OPT_CLOSE_ON_FREE);
		assert_retval(bev, AGERR_INVALID_PARAM);

		CHANNELINFO* info = _new_channel_info(bev, channel_id, TRUE);
		assert_retval(info, AGERR_INVALID_PARAM);

		//连接服务器
		bufferevent_socket_connect(bev,(struct sockaddr*)&serv,sizeof(serv));

		//设置回调
		bufferevent_setcb(bev, _conn_channel_readcb, NULL, _client_event_cb, info);

		//设置回调生效
		bufferevent_enable(bev, EV_READ);

		//创建事件
		struct event* ev = event_new(g_event_base, -1, EV_TIMEOUT | EV_PERSIST,
			_connect_cb, info);
		assert_retval(ev, AGERR_INVALID_PARAM);

		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 0;
		tv.tv_usec = 1000 * WBUS_MAX_CONNECT_TIMER;
		//添加事件
		event_add(ev, &tv);

		return AGERR_SUCCESS;
	}

	static int32_t _network_init(TAPPCTX* a_pstCtx)
	{
		if (!a_pstCtx) return AGERR_NULL_POINTER;

		struct sockaddr_in sin;

#if defined(_WIN32)||defined(_WIN64)
		WSADATA wsa_data;
		WSAStartup(0x0201, &wsa_data);
#endif
		g_event_base = event_base_new();
		if (!g_event_base)
		{
			//error_tlog("could not initialize libevent!")
			return AGERR_INVALID_PARAM;
		}
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(g_local_info.bus_addr.local_port);

		g_event_listener[0] = evconnlistener_new_bind(g_event_base, listener_channel_cb, NULL,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

		if (!g_event_listener[0])
		{
			//error_tlog("Count not create a listener!");
			return AGERR_INVALID_PARAM;
		}

		sin.sin_port = htons(g_local_info.peer_addr.peer_port);
		g_event_listener[1] = evconnlistener_new_bind(g_event_base, listener_peer_cb, NULL,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

		if (!g_event_listener[1])
		{
			//error_tlog("Count not create a listener!");
			return AGERR_INVALID_PARAM;
		}

		_set_reconnect_timer();
		return AGERR_SUCCESS;

	}

	static int32_t _network_uninit()
	{
		struct timeval delay = { 1,0 };
		event_base_loopexit(g_event_base, &delay);
		evconnlistener_free(g_event_listener[0]);
		evconnlistener_free(g_event_listener[1]);
		event_base_free(g_event_base);
		free(g_peer_recv_buff);

		return AGERR_SUCCESS;
	}

	static void _set_reconnect_timer()
	{
		memset(&g_reconnect_timer, 0, sizeof(g_reconnect_timer));
		g_reconnect_timer.interval_time = g_local_info.bus_addr.reconnect_interval;
		g_reconnect_timer.remain_times = g_local_info.bus_addr.reconnect_timer == -1 ?
			INT_MAX : g_local_info.bus_addr.reconnect_timer;
		g_reconnect_timer.next_time = cur_time();
	}

	static void _close_link(LINKBASE* base, struct bufferevent* bev)
	{
		CHANNELINFO* channel_info = NULL;
		LOG_PROCESS_ERROR(base);

		//!peer
		if (base->type == AGBUS_CLIENT_ADDR)
		{
			PEERINFO* peer_info = (PEERINFO*)base;
			if (peer_info)
			{
				//infor_tlog("peer <%d> will been closed!", peer_info->peer_id);
				printf("peer <%d> will been closed!", peer_info->peer_id);
				if (g_app_ctx && g_app_ctx->pfnDisconnect)
				{
					g_app_ctx->pfnDisconnect(g_app_ctx, g_app_arg,peer_info);
				}
				g_peer_info_pool.delete_object(peer_info);
				bufferevent_free(peer_info->bufvents);
			}
			return;
		}

		//!else channel
		channel_info = (CHANNELINFO*)base;
		if (channel_info)
		{
			WBUSADDR opposite_ip = get_opposite_addr(channel_info->dst_virtual_addr, channel_info->src_virtual_addr);
			//infor_tlog("channel <%d> dst_addr<%s> will been closed!", channel_info->channel_id, wbus_addr_ntoa(opposite_ip));
			printf("channel <%d> dst_addr<%d> will been closed!", channel_info->channel_id,(opposite_ip));
			on_channel_closed(channel_info);
		}
	Exit0:
		if (!base && bev)
		{
			bufferevent_free(bev);
		}
		return;
	}


	static void listener_channel_cb(struct evconnlistener* listener, evutil_socket_t fd,
		struct sockaddr* sa, int socklen, void*user_data)
	{
		if (!listener || !sa)
		{
			//error_tlog("listener_cb , params havr null pointer!");
			printf("listener_cb , params havr null pointer!");
			return;
		}
		struct bufferevent* bev;
		bev = bufferevent_socket_new(g_event_base, fd, BEV_OPT_CLOSE_ON_FREE);
		assert_retnone(bev);

		bufferevent_setcb(bev, _conn_channel_readcb, NULL, conn_eventcb, NULL);
		bufferevent_enable(bev, EV_WRITE | EV_READ);
	}

	static void listener_peer_cb(struct evconnlistener* listener, evutil_socket_t fd,
		struct sockaddr* sa, int socklen, void* user_data)
	{
		if (!listener || !sa)
		{
			//error_tlog("listener_cb ,params have null pointer!");
			printf("listener_cb ,params have null pointer!");
			return;
		}
		struct bufferevent* bev;
		bev = bufferevent_socket_new(g_event_base, fd, BEV_OPT_CLOSE_ON_FREE);
		assert_retnone(bev);

		//! client to server peer
		//peer 是客户端连接上来的数量，从1000开始自增
		PEERINFO* info = _new_peer_info(bev, inet_addr(sa->sa_data),++g_local_info.peer_addr.peer_id);
		assert_retnone(info);

		bufferevent_setcb(bev, _conn_peer_readcb, NULL, conn_eventcb, info);
		bufferevent_enable(bev, EV_WRITE | EV_READ);
		return;
	
	}

	static void conn_writecb(struct bufferevent* bev, void* user_data)
	{

	}

	BOOL push_event(NETWORKEVENT& evt, int type, struct bufferevent* bev)
	{
		switch (type)
		{
		case NET_CHANNEL_CONNECT:
		{
			AGBUSBASEHEAD* head = _add_base_head((const char*)&evt, sizeof(evt.cce));
			assert_retval(head, false);
			bufferevent_write(bev, head, head->msg_size + sizeof(AGBUSBASEHEAD));
			return true;
		}

		case NET_PEER_CONNECT:
		{
			AGBUSBASEHEAD* head = _add_base_head((const char*)&evt, sizeof(evt.pce));
			assert_retval(head, false);
			bufferevent_write(bev, (const char*)head, head->msg_size + sizeof(AGBUSBASEHEAD));
			return true;
		}
		default:
			break;
		}

		return false;
	}


	BOOL pop_event(NETWORKEVENT& evt, int type, char* data, int len)
	{
		switch (type)
		{
		case NET_CHANNEL_CONNECT:
		{
			assert_retval(len >= sizeof(evt.cce), false);
			evt = *(NETWORKEVENT*)data;
			break;
		}

		case NET_PEER_CONNECT:
		{
			assert_retval(len >= sizeof(evt.pce), false);
			evt = *(NETWORKEVENT*)data;
			break;
		}

		default:
			break;
		}

		return true;
	}

	static void _conn_channel_readcb(struct bufferevent* bev, void* user_data)
	{
		CHANNELINFO* info = static_cast<CHANNELINFO*>(user_data);

		struct evbuffer* input = bufferevent_get_input(bev);
		if (!input) return;

		static char buf[MAX_PACKET_SIZE];
		memset(buf, 0x0, sizeof(buf));
		AGCHANNELHEAD* head = (AGCHANNELHEAD*)buf;

		while (true)
		{
			ev_ssize_t resv_total = evbuffer_get_length(input);
			if (resv_total <= sizeof(head->sv_head))
				break;

			ev_ssize_t len = evbuffer_copyout(input, &head->sv_head, sizeof(head->sv_head));
			if (len < 0)
				break;

			assert_retnone(len == sizeof(head->sv_head));

			if (sizeof(head->sv_head) + head->sv_head.msg_size > resv_total)
				break;

			size_t package_len = head->sv_head.msg_size + sizeof(head->sv_head);
			ev_ssize_t readlen = bufferevent_read(bev, buf + sizeof(AGCHANNELHEAD) - sizeof(head->sv_head), package_len);
			if (!info && readlen > 0)
			{
				NETWORKEVENT evt;
				assert_retnone(pop_event(evt, NET_CHANNEL_CONNECT, buf + sizeof(AGCHANNELHEAD), readlen - sizeof(head->sv_head)));

				info = _new_channel_info(bev, MAKE_WBUS_CHANNEL_ID(evt.cce.virtual_ip, evt.cce.port));
				assert_retnone(info);

				bufferevent_setcb(bev, _conn_peer_readcb, NULL, conn_eventcb, (void*)info);
				on_channel_connected(info);
				continue;
			}

			head->channel_id = info->channel_id;
			if(readlen > 0)
			{
				assert_retnone(info->recv_buf);

				if (readlen >= info->recv_buf->remain())
				{
					//error_tlog("buffer is full ,socket will been closed!");
					printf("buffer is full ,socket will been closed!");
					//error_tlog("readlen = %d,recv_buf_remain = %d,msg_size = %d", readlen, info->recv_buf->remain(), head->sv_head.msg_size);
					printf("readlen = %d,recv_buf_remain = %d,msg_size = %d", readlen, info->recv_buf->remain(), head->sv_head.msg_size);
					_close_link(info, bev);
					return;
				}

			}
			else if (readlen < 0)
			{
				if ((errno != EAGAIN) && (errno != EINPROGRESS))
				{
					//出错
					//error_tlog("conn_readcb,recv fail ,errno %d,%s", errno, strerror(errno));
					printf("conn_readcb,recv fail ,errno %d,%s", errno, strerror(errno));
					_close_link(info, bev);
					return;
				}
			}
			else
			{

			}

		}

	}

	static void _conn_peer_readcb(struct bufferevent* bev, void* user_data)
	{
		PEERINFO* info = (PEERINFO*)user_data;
		struct evbuffer* input = bufferevent_get_input(bev);
		if (!input) return;

		static char buf[MAX_PACKET_SIZE];
		memset(buf, 0x0, sizeof(buf));
		AGPEERHEAD* peer_head = (AGPEERHEAD*)buf;

		while (true)
		{
			ev_ssize_t resv_total = evbuffer_get_length(input);
			if(resv_total <= sizeof(peer_head->cl_head))
				break;

			ev_ssize_t len = evbuffer_copyout(input, &peer_head->cl_head, sizeof(peer_head->cl_head));
			if (len < 0)
				break;

			assert_retnone(len == sizeof(peer_head->cl_head));

			if (sizeof(peer_head->cl_head) + peer_head->cl_head.msg_size > resv_total)
				break;
			peer_head->peer_id = info->peer_id;

			size_t package_len = peer_head->cl_head.msg_size + sizeof(peer_head->cl_head);
			ev_ssize_t readlen = bufferevent_read(bev, (void*)&(peer_head->cl_head), package_len);

			assert_retnone(info);

			if (readlen > 0)
			{
				readlen = readlen + (sizeof(AGPEERHEAD) - sizeof(AGBUSBASEHEAD));
				assert_retnone(g_peer_recv_buff);
				if (readlen >= g_peer_recv_buff->remain())
				{
					//error_tlog("buffer is full ,socket will been closed!");
					printf("buffer is full ,socket will been closed!");
					_close_link(info, bev);
					return;
				}

				g_peer_recv_buff->write_arr(buf, readlen);
				info->recv_pkg_cnt++;

			}
			else if (readlen < 0)
			{
				if ((errno != EAGAIN) && (errno != EINPROGRESS))
				{
					//出错
					//error_tlog("conn_readcb,recv fail ,errno %d,%s", errno, strerror(errno));
					printf("conn_readcb,recv fail ,errno %d,%s", errno, strerror(errno));
					_close_link(info, bev);
					return;
				}
			}
			else
			{

			}

		}

	}

	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data)
	{
		if (events & BEV_EVENT_CONNECTED)
		{
			//infor_tlog("socket is connected!");
			printf("socket is connected!");
			return;
		}

		//error_tlog("got an error<%d> on the connection ", events);
		printf("got an error<%d> on the connection ", events);
		_close_link((LINKBASE*)user_data, bev);
	}

	void network_loop()
	{
		if (g_reconnect_timer.next_time <= cur_time() && g_reconnect_timer.remain_times > 0)
		{
			reconnect_by_cfg();
			g_reconnect_timer.remain_times -= 1;
			g_reconnect_timer.next_time = cur_time() + (g_reconnect_timer.interval_time / 1000);
		}
	}

	PEERINFO* find_peer(PEERID peer_id)
	{
		return g_peer_info_pool.find(peer_id);
	}


	int32_t peer_send(PEERID peer_id, char* buf, size_t len)
	{
		AGBUSBASEHEAD* head = _add_base_head(buf, len);
		assert_retval(head, AGERR_NET_SEND_OVERFLOW);

		PEERINFO* pPeer = g_peer_info_pool.find(peer_id);
		if (!pPeer || !pPeer->bufvents) return AGERR_INVALID_PARAM;

		struct evbuffer* output = bufferevent_get_output(pPeer->bufvents);
		assert_retval(output, AGERR_INVALID_PARAM);

		int ret = bufferevent_write(pPeer->bufvents, (void*)head, head->msg_size + sizeof(AGBUSBASEHEAD));
		assert_retval(ret != -1, AGERR_INVALID_PARAM);

		pPeer->send_pkg_cnt++;
		
		return AGERR_SUCCESS;

	}

	int32_t peer_sendv(PEERID peer_id, const struct iovec* a_ptVector, const int a_iVecCnt)
	{
		int send_buf_len = 0;
		static char send_buf[MAX_PACKET_SIZE];
		for (int i = 0; i < a_iVecCnt; ++i)
		{
			assert_retval(send_buf_len + a_ptVector[i].iov_len < sizeof(send_buf), AGERR_NET_SEND_OVERFLOW);
			int len = snprintf(send_buf + send_buf_len, sizeof(send_buf) - send_buf_len,
				"%s", (char*)a_ptVector[i].iov_base);

			assert_retval(len == a_ptVector[i].iov_len, AGERR_INVALID_PARAM);
			send_buf_len += len;

		}
		return peer_send(peer_id, send_buf, send_buf_len);
	}

	int32_t get_channel_addr(WBUSADDR* out_addrs, int& out_cnt, int svr_type /* = -1 */)
	{
		int32_t find_cnt = 0;
		if (!out_addrs || out_cnt <= 0)
			return AGERR_NULL_POINTER;

		auto it = g_channel_info_map.begin();
		for (; it != g_channel_info_map.end(); it++)
		{
			auto info = it->second;
			if (!info || find_cnt >= out_cnt)
				return AGERR_NULL_POINTER;

			if (svr_type == -1 || (svr_type == GET_SERVER_TYPE(it->first)))
			{
				out_addrs[find_cnt] = it->first;
				find_cnt++;
			}
		}
		out_cnt = find_cnt;
		return AGERR_SUCCESS;
	}

	CHANNELINFO* find_dst_channel(WBUSADDR src_id, WBUSADDR dst_addr, BOOL with_recv_data)
	{
		auto it = g_channel_info_map.begin();
		for (; it != g_channel_info_map.end(); ++it)
		{
			if (it->first == dst_addr || dst_addr == WBUS_ADDR_ALL)
				return it->second;
		}
		return NULL;

	}

	CHANNELINFO* find_channel(CHANNELID channel_id)
	{
		return g_channel_info_pool.find(channel_id);
	}

	int32_t channel_send(CHANNELID channel_id, char* buf, size_t len)
	{
		AGBUSBASEHEAD* head = _add_base_head(buf, len);
		assert_retval(head, AGERR_NET_SEND_OVERFLOW);

		CHANNELINFO* pChannel = g_channel_info_pool.find(channel_id);
		if (!pChannel || !pChannel->bufvents || !pChannel->is_connected)
			return AGERR_INVALID_PARAM;

		struct evbuffer* output = bufferevent_get_output(pChannel->bufvents);
		assert_retval(output, AGERR_INVALID_PARAM);

		pChannel->send_pkg_cnt++;

		return AGERR_SUCCESS;
	}

	int32_t channel_sendv(CHANNELID channel_id, const struct iovec* a_ptVector, const int a_iVecCnt)
	{
		int send_buf_len = 0;
		static char send_buf[MAX_PACKET_SIZE];
		for (int i = 0; i < a_iVecCnt; ++i)
		{
			assert_retval(send_buf_len + a_ptVector[i].iov_len < sizeof(send_buf), AGERR_NET_SEND_OVERFLOW);

			memcpy(send_buf + send_buf_len, (char*)a_ptVector[i].iov_base, a_ptVector[i].iov_len);
			send_buf_len += a_ptVector[i].iov_len;
		}
		return channel_send(channel_id, send_buf, (size_t)send_buf_len);
	}

	WBUSADDR get_local_addr()
	{
		return g_local_info.bus_addr.local_ip > 0 ? g_local_info.bus_addr.local_ip : 0;
	}

	uint16_t get_local_port()
	{
		return g_local_info.bus_addr.local_port > 0 ? g_local_info.bus_addr.local_port : 0;
	}

	CRingBuff* get_peer_buf()
	{
		return g_peer_recv_buff;
	}

	CRingBuff* get_channel_buf()
	{
		return g_channel_recv_buff;
	}

	static AGBUSBASEHEAD* _add_base_head(const char* buf, int len)
	{
		assert_retval(sizeof(AGBUSBASEHEAD) + len <= MAX_PACKET_SIZE, NULL);
		static char tbuf[MAX_PACKET_SIZE];


		AGBUSBASEHEAD* head = (AGBUSBASEHEAD*)tbuf;
		head->msg_size = len;
		memcpy(head->msg, buf, head->msg_size);
		return head;
	}

	static char* _del_head(AGBUSBASEHEAD* head)
	{
		assert_retval(head, NULL);
		assert_retval(head->msg_size <= MAX_PACKET_SIZE, NULL);
		return (char*)head->msg;
	}

	int32_t new_connect(char* ip, int port)
	{
		return 0;
	}

	int32_t on_channel_connected(CHANNELINFO* info)
	{
		if (!info) return AGERR_INAVLAID_PARAM;

		info->is_connected = true;
		on_cfg_channel_connected(info->channel_id);

		if (g_app_ctx && g_app_ctx->pfnConnect)
			g_app_ctx->pfnConnect(g_app_ctx, g_app_arg, info);
	}

	int32_t on_channel_closed(CHANNELINFO* info)
	{
		if (!info) return AGERR_INAVLAID_PARAM;

		int opposite_addr = get_opposite_addr(info->src_virtual_addr, info->dst_virtual_addr);
		g_channel_info_map.erase(opposite_addr);
		g_channel_info_pool.delete_object(info);
		bufferevent_free(info->bufvents);
		if (g_app_ctx && g_app_ctx->pfnDisconnect)
			g_app_ctx->pfnDisconnect(g_app_ctx, g_app_arg, info);

		on_cfg_channel_closed(info->channel_id);

	}

	uint32_t round_up_2power(int32_t x)
	{
		return  pow(2, ceil(log(x) / log(2)));
	}
	int64_t cur_time()
	{
		struct timeval tv;
		evutil_gettimeofday(&tv, NULL);
		return tv.tv_sec;
	}
#ifdef __cplusplus
}
#endif




















