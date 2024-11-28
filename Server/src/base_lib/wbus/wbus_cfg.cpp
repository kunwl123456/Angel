#include "stdafx.h"
#include <app/tapp.h>
#include <wbus_def.h>
#include <wbus_cfg.h>
#include <wbus_network.h>
#include <define/server_def.h>
#include "tinyxml.h"
#include "tinystr.h"


#include "base_lib/id_object_pool.h"
#include "base_lib/error_code.h"
#include "base_lib/process_error.h"
//#include "base_lib/container/fixed_map.h"

#ifdef __cplusplus
extern "C" {
#endif
	static CIDObjectPool<AGBUSPAIRNETADDR> g_server_bus_addr_pool;
	//static sk::fixed_map<CHANNELID, AGBUSPAIRNETADDR*, MAX_CHANNEL_CNT> g_server_bubs_addr_map;
	static std::map<CHANNELID, AGBUSPAIRNETADDR*> g_server_bubs_addr_map;

	int32_t _init_data(BOOL resume)
	{
		g_server_bubs_addr_map.clear();
		//g_server_bubs_addr_map.init();
		if (resume)
		{
			g_server_bus_addr_pool.restore(PAIR_SERVER_NAME);
			auto it = g_server_bus_addr_pool.get_first_obj();
			for (; it != NULL; it = g_server_bus_addr_pool.get_next_obj())
				g_server_bubs_addr_map[it->channel_id] = it;
			return AGERR_SUCCESS;
		}

		g_server_bus_addr_pool.init(MAX_CHANNEL_CNT, PAIR_SERVER_NAME);
		return AGERR_SUCCESS;
	}

	int32_t init_wbus_cfg(const char* name, BOOL resume, AGBUSLOCALINFO& info)
	{
		if (!name) return errParamInvalid;
		TiXmlDocument doc;
		const TiXmlElement*  elm = NULL;
		BOOL ok = doc.LoadFile();
		if (!ok)
		{
			printf("server config file:%s load failed,error :%s\n", name, doc.ErrorDesc());
			return AGERR_SERVER_CONF_LOAD_FAILED;
		}
		const TiXmlElement* root = doc.FirstChildElement();
		assert_retval(root, AGERR_SERVER_CONF_LOAD_FAILED);
		elm = root->FirstChildElement();
		for (; elm != NULL; elm = elm->NextSiblingElement())
		{
			AGBUSPAIRNETADDR bus_pair = { 0 };
			char temp_ip1[16] = { 0 };
			char temp_ip2[16] = { 0 };
			if (strcmp(elm->Value(), "Peers") == 0)
			{
				int v_ip1 = 0, v_ip2 = 0, port = 0;
				wbus_addr_aton(elm->Attribute("Address1"), &v_ip1);
				wbus_addr_aton(elm->Attribute("Address2"), &v_ip2);
				if (v_ip1 == get_local_addr())
				{
					sscanf(elm->Attribute("NetAddress1"), "%[^:]:%d", &temp_ip1, &port);
					info.peer_addr.recv_buff_size = atoi(elm->Attribute("RecvSize"));
					info.peer_addr.peer_id = atoi(elm->Attribute("PeerID"));
					info.peer_addr.peer_port = port;
				}
				else if(v_ip2 == get_local_addr())
				{
					sscanf(elm->Attribute("NetAddress2"), "%[^:]:%d", &temp_ip2, &port);
					info.peer_addr.recv_buff_size = atoi(elm->Attribute("RecvSize"));
					info.peer_addr.peer_id = atoi(elm->Attribute("PeerID"));
					info.peer_addr.peer_port = port;
				}
			}
			else if (strcmp(elm->Value(),"ReConnect")== 0)
			{
				info.bus_addr.reconnect_interval = atoi(elm->Attribute("Interval"));
				info.bus_addr.reconnect_timer = atoi(elm->Attribute("Times"));
			}
			else if (strcmp(elm->Value(), "Channels") == 0)
			{
				wbus_addr_aton(elm->Attribute("Address1"), &bus_pair.bus1.virtual_addr);
				wbus_addr_aton(elm->Attribute("Address2"), &bus_pair.bus2.virtual_addr);
				
				sscanf(elm->Attribute("NetAddress1"), "%[^:]:%d", &temp_ip1, &bus_pair.bus1.port);
				bus_pair.bus1.net_ip = inet_addr(temp_ip1);

				sscanf(elm->Attribute("NetAddress2"), "%[^:]:%d", &temp_ip2, &bus_pair.bus2.port);
				bus_pair.bus2.net_ip = inet_addr(temp_ip2);

				if (bus_pair.bus1.virtual_addr == get_local_addr())
				{
					bus_pair.channel_id = MAKE_WBUS_CHANNEL_ID(bus_pair.bus2.virtual_addr, bus_pair.bus2.port);
					info.bus_addr.local_port = bus_pair.bus1.port;
				}
				else if (bus_pair.bus2.virtual_addr == get_local_addr())
				{
					bus_pair.channel_id = MAKE_WBUS_CHANNEL_ID(bus_pair.bus1.virtual_addr, bus_pair.bus1.port);
					info.bus_addr.local_port = bus_pair.bus1.port;
				}
				else
				{
					continue;
				}
				
				bus_pair.send_bus_size = round_up_2power(atoi(elm->Attribute("SendSize")));
				bus_pair.recv_buf_size = round_up_2power(atoi(elm->Attribute("RecvSize")));
				AGBUSPAIRNETADDR* addr = g_server_bus_addr_pool.new_object(bus_pair.channel_id);
				assert_retval(addr, AGERR_SUCCESS);
				*addr = bus_pair;
				g_server_bubs_addr_map[bus_pair.channel_id] = addr;
				//infor_tlog("addr new server bus_addr <%s> channel_id <%llu>", wbus_addr_ntoa(bus_pair.bus1.virtual_addr), bus_pair.channel_id);
				printf("addr new server bus_addr <%s> channel_id <%llu>", wbus_addr_ntoa(bus_pair.bus1.virtual_addr), bus_pair.channel_id);

			}

		}
	}

	void uninit_wbus_cfg()
	{
		//g_server_bubs_addr_map.uninit();
		g_server_bubs_addr_map.clear();
		g_server_bus_addr_pool.uninit();
		return;
	}

	AGBUSPAIRNETADDR* get_cfg_addr_by_channel(const uint64_t channel_id)
	{
		return g_server_bus_addr_pool.find(channel_id);
	}

	int32_t on_cfg_channel_closed(const CHANNELID& channel_id)
	{
		AGBUSPAIRNETADDR* pair_addr = g_server_bus_addr_pool.find(channel_id);
		if (!pair_addr) return AGERR_INVALID_PARAM;
		pair_addr->connected = false;
		return AGERR_SUCCESS;
	}

	int32_t on_cfg_channel_connected(const CHANNELID& channel_id)
	{
		AGBUSPAIRNETADDR* pair_addr = g_server_bus_addr_pool.find(channel_id);
		if (!pair_addr) return AGERR_INVALID_PARAM;

		pair_addr->connected = true;
		return AGERR_SUCCESS;
	
	}

	int32_t get_wbus_cfg_addrs(WBUSADDR* (&addrs), int32_t& count)
	{
		count = 0;

		auto it = g_server_bubs_addr_map.begin();
		for (; it!=g_server_bubs_addr_map.end(); ++it)
		{
			if(!it->second) continue;
			AGBUSPAIRNETADDR& data = *(it->second);
			addrs[count] = data.bus1.virtual_addr == get_local_addr() ? data.bus2.virtual_addr : data.bus1.virtual_addr;
			count++;
		}

		return errSuccess;
	}


	BOOL is_connected_all()
	{
		auto it = g_server_bubs_addr_map.begin();
		for (; it != g_server_bubs_addr_map.end(); it++)
		{
			if(!it->second) continue;
			AGBUSPAIRNETADDR& data = *(it->second);
			if (!data.connected) return FALSE;
		}
		return TRUE;
	}

	int32_t reconnect_by_cfg()
	{
		auto it = g_server_bubs_addr_map.begin();
		for (; it != g_server_bubs_addr_map.end(); it++)
		{
			AGBUSPAIRNETADDR& data = *(it->second);
			if (data.bus1.virtual_addr == get_local_addr() && data.connected == false)
				data.connected = new_connect_net_addr(data.channel_id) == AGERR_SUCCESS;
		}
		return errSuccess;
	}


#ifdef __cplusplus
}
#endif












