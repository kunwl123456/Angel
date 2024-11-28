#ifndef _WBUS_DEF_H__
#define _WBUS_DEF_H__

#include "def/type_def.h"

#if defined(_WIN32) || defined(_WIN64)
struct iovec {
	void*  iov_base;    /*starting address*/
	size_t iov_len;     /*number of bytes*/
};
#endif

/*
server_addr:32
area: 6
type: 6 SERVER_TYPE
index:10
second:10
*/
#define GET_SERVER_AREA(__addr__)   (((__addr__) >>2) & 0x3F)//  0011 1111
#define GET_SERVER_ZONE(__addr__)   (((__addr__) & 0x3) << 8) | (((__addr__) >> 8) &0xFF) //
#define GET_SERVER_TYPE(__addr__)   (((__addr__) >> 18) & 0x3F)
#define GET_SERVER_INDEX(__addr__)  ((((__addr__) >> 16) & 0x3) << 8) | (((__addr__) >> 24) &0xFF)

#define MAKE_SERVER_ADDR(__area__,__zone__,__type__,__index__)\
		(((((__index__) + 0ULL) &0xFF ) << 24)                \
		|(((__index__)&0x300)<<8)                             \
		|(((__type__)&0x3F)<<18)                              \
		|(((__area__)&0x3F)<<2)                               \
		|(((__zone__)&0xFF)<<8)                               \
		|((__zone__)&0x300)>>8)


#define WBUS_CHANNEL_HEAD_RESERVE_NUM  (4)
#define WBUS_MAX_CHANNEL_NUM_PREHOST   (300)

#define MAX_CHANNEL_CNT                          (256)   //最大通道数
#define MAX_PEER_CNT                             (10240) //最大peer数
#define WBUS_MAX_CHNNEL_COUNT_PREHANDLE          (WBUS_MAX_CHANNEL_NUM_PREHOST)
#define WBUS_MAX_BIND_ADDR_NUM_PREHANDLE         (32)
#define WBUS_CHANNEL_GENERATE_ID                 (1000)
#define WBUS_MAX_CONNECT_TIMER                   (50)

#define FRAMEHEAD_MAX_BATCH_IDENT                 (512)   /*最大组播个数*/
#define FRAMEHEAD_EXTRA_IP                        (1)
#define FRAMEHEAD_TIMERSTAMP_TIMEVAL              (1)
#define SERLIZER_NAME_LEN                         (32)
#define NETTRANS_NAME_LEN                         (32)
#define FRAMEHEAD_SET_ROUTING                     (1)
#define MAX_BROADCAST_ALL_MSG_LEN                 (10240) /*一个广播消息包的最大大小*/
#define MAX_BROADCAST_ALL_MSG_NUM                 (100)   /*广播消息包缓冲区能容纳的广播消息包的最大个数*/
#define MAX_CLIENT_NUM_ONCE_SEND                  (100)   /*一次给多少个客户端发送广播包*/
#define MAX_MUTICAST_MSG_LEN                      (10240) /*一个组播消息包的最大大小*/
#define MAX_MUTICAST_MSG_NUM                      (1024)  /*组播消息包缓冲区能容纳的组播消息包的最大个数*/
#define FRAMEHEAD_MAX_ROUTINE_ARGS_LEN            (128)   
#define FRAMEHEAD_CONNECTKEY_LEN                  (16)
#define FRAMEHEAD_IDENTITY_LEN                    (16)
#define FRAMEHEAD_MAX_SIG_LEN                     (1024)
#define FRAMEHEAD_MAX_SIG_LEN_BIG                 (4096)
#define WEN_SKEY_LEN                              (128)  /*SKEY长度，10+1，留了一个"\0"的位置*/
#define WEB_ACCOUNT_LEN                           (31)   /*账户名称，溜了一个'\0'的位置*/
#define WEB_TOKEN_LEN                             (201)  /*Token长度，留了一个"\0"的位置*/
#define WEB_PASSPORT_LEN                          (20)   /* 增值位，20bytes */
#define WEB_OPENID_LEN                            (128)  /*qq互联openid长度*/
#define WEB_ACCESS_TOKEN_LEN                      (256)  /*qq互联accesstoken长度*/
#define FRAMEHEAD_MAX_ID_LENGTH                   (256)  /*字符串账号最大长度*/
#define GATEWAY_MULTICAST_GROUPNAME_LEN           (64)   /*组播组名字最大允许长度64字节*/
#define GATEWAY_WAP_MAX_KEY_LEN                   (30)
#define GATEWAY_WAP_MAX_VALUE_LEN                 (200)
#define GATEWAY_WAP_MAX_URI_LEN                   (1024)
#define GATEWAY_WAP_MAX_BODY_LEN                  (102400)
#define GATEWAY_WAP_MAX_PARAMS_COU                (20)
#define GATEWAY_WAP_MAX_HEADERS_COU               (20)
#define GATEWAY_WAP_MAX_COOKIES_COU               (20)
#define MAX_PACKET_SIZE                           (1024*64)
#define MAX_LOCAL_CHANNEL_RECEV_BUFF_SIZE         (1024*1024*64)


/* tagPRAMEHEAD_CMD_ID */
enum tagFRAMEHEAD_CMD_ID
{
	FRAMEHEAD_CMD_START  = 0,   /* 连接初始包，双向 */
	FRAMEHEAD_CMD_STOP   = 1,   /* 连接断开包，双向 */
	FRAMEHEAD_CMD_INPROC = 2,   /* 通信包，双向 */
	FRAMEHEAD_CMD_RELAY  = 3,   /* 重连包，双向 */
	FRAMEHEAD_CMD_NOTIFY = 4,   /* 预占连接包，双向 */

	PRAMEHEAD_CMD_MULTICAST_REMOVE_IDENT_REQ  = 10,     /* 组播组删除连接请求 */
	PRAMEHEAD_CMD_MULTICAST_REMOVE_IDENT_RESP = 11,     /* 组播组删除连接响应 */
	PRAMEHEAD_CMD_MULTICAST_ADD_IDENT_REQ     = 12,     /* 在组播组添加连接请求 */
	PRAMEHEAD_CMD_MULTICAST_ADD_IDENT_RESP    = 13,     /* 在组播组添加连接响应 */
	PRAMEHEAD_CMD_MULTICAST_DELIVER_MSG_REQ   = 14,     /* 发送组播消息 */
	PRAMEHEAD_CMD_BROADCAST                   = 15,     /* 广播包，下行 */
	PRAMEHEAD_CMD_BROADCAST_RSP               = 16,     /* 广播答应包，用于回应广播包是否能发送成功，上行 */

};

/* FRAMEHEAD_STOP_REASON */
enum tagFRAMEHEAD_STOP_REASON
{
	FRAMEHEAD_REASON_NONE = 0,
	FRAMEHEAD_REASON_IDLE_CLOSE = 1,
	FRAMEHEAD_REASON_PEER_CLOSE = 2,
	FRAMEHEAD_REASON_NETWORK_FAIL = 3,
	FRAMEHEAD_REASON_BAD_PKGLEN = 4,
	FRAMEHEAD_REASON_EXCEED_LIMIT = 5,
	FRAMEHEAD_REASON_TCONND_SHUTDOWN = 6,
	FRAMEHEAD_REASON_SELF_CLOSE = 7,
	FRAMEHEAD_REASON_AUTH_PATH = 8,
	FRAMEHEAD_REASON_SYNACK_FAIL = 9,
	FRAMEHEAD_REASON_WR_BLOCKED = 10,
	FRAMEHEAD_REASON_SEQUENCE_INVALID = 11,
	FRAMEHEAD_REASON_TRANS_RELAY = 12,
	FRAMEHEAD_REASON_TRANS_LOST = 13,
	FRAMEHEAD_REASON_RELAY_FAIL = 14,
	FRAMEHEAD_REASON_SESSION_RENEW_FAIL = 15,
	FRAMEHEAD_REASON_RECV_BUFF_FULL = 16,
	FRAMEHEAD_REASON_UNPACK_FAIL = 17,
	FRAMEHEAD_REASON_INVALID_PKG = 18,
	FRAMEHEAD_REASON_INVALID_SKEY = 19,
	FRAMEHEAD_REASON_VERIFY_DUP = 20,
	FRAMEHEAD_REASON_CLIENT_CLOSE = 21,
	FRAMEHEAD_REASON_PRE_RELAY_FAIL = 22,
	FRAMEHEAD_REASON_SYSTEM_ERROR = 23,
	FRAMEHEAD_REASON_CLIENT_RECONNECT = 24,
	FRAMEHEAD_REASON_GEN_KEY_FAIL = 25,
	FRAMEHEAD_REASON_ID_MAPPING_FAIL = 26,
	FRAMEHEAD_REASON_SVR_CONFIRM_TIME_OUT = 27,
	FRAMEHEAD_REASON_ROUTING_FAIL = 28,
	FRAMEHEAD_REASON_TCONND_UNREACHABLE = 29,
	FRAMEHEAD_REASON_LOGIN_ON_NEW_SVR = 30,
	FRAMEHEAD_REASON_LOGIN_ON_FIND_ROUTE = 31,
	FRAMEHEAD_REASON_CLUSTER_WAIT_QUEUE_TIME_OUT = 32,
	FRAMEHEAD_REASON_CLUSTER_PUSH_QUEUE_FAILED = 33,
	FRAMEHEAD_REASON_AUTH_PARAM_INVALID_IN_REQUEST = 34,
	FRAMEHEAD_REASON_GET_CLUSTER_ROUTE_FAILED = 35,
	FRAMEHEAD_REASON_CLOSE_DETECT = 36,
	FRAMEHEAD_REASON_COUNT = 37,       /*加错误码后记得改内部和外部两个tconnapi.cpp里面的tconnd_close_string[]*/
};

/*tagClientAcount*/
enum tagACCOUNT
{
	ACCOUNT_NONE = 0,
	ACCOUNT_QQ_UIN = 1,
	ACCOUNT_QQ_UIN_PTLOGIN = 2,
	ACCOUNT_WX_OPENID = 4098,
	ACCOUNT_QQ_OPENID = 4099,
	ACCOUNT_OPENID_HL = 4100,
	ACCOUNT_IOS_QUEST = 8193,
	ACCOUNT_FACEBOOK = 32769,
	//..未完待续
};


/* tagCLIENT_TYPE */
enum tagCLIENT_TYPE
{
	CLIENT_TYPE_PC = 0,
	CLIENT_TYPE_MOBILE = 1,
	CLIENT_TYPE_ANDROID = 101,
	CLIENT_TYPE_IOS = 102,
};


/* tagBROCAST_TYPE */
enum tagBROCAST_TYPE
{
	BROCAST_TYPE_ONE,
	BROCAST_TYPE_GROUP,
	BROCAST_TYPE_ALL,
};

enum tagWBUS_TYPE
{
	AGBUS_CLIENT_ADDR,  // client addr -->peer
	AGBUS_SERVER_ADDR,  // server addr -->channel
};

enum tagNETWORK_EVENT_TYPE
{
	NET_CHANNEL_CONNECT,
	NET_PEER_CONNECT,
};

//////////////////////////////////////////////////////////////////////////
//! new define

#pragma pack(push,1)
typedef int WBUSADDR;           /* wbus  系统通信地址  -- virtuial addr */
typedef int* LPWBUSADDR;       /* wbus   系统通信地址  */
typedef int PEERID;
typedef uint64_t CHANNELID;

/*

	bus: local virtual addr
	channel: server <-> server
	peer: client<->server

*/

typedef struct tag_net_time_stamp
{
	int iIsAvailable;    /* 前一个消息是否有时间戳 */
	long long llSec;     /* 时间戳值   seconds部分 */
	unsigned int dwUSec; /* 时间戳值   microseconds部分 */
	unsigned int dwCumm; /* TBUS处理该消息所用的时间 */
}NETTIMESTAP;



struct tag_link_base
{
	int type;
};

typedef struct tag_link_base LINKBASE;

struct tag_channel_info :public LINKBASE
{
	CHANNELID channel_id;

	uint16_t src_port;
	int src_addr;   // net_addr_id
	int src_virtual_addr; //bus_addr

	uint16_t dst_port;
	int dst_addr;   //net_addr_id
	int dst_virtual_addr;

	int send_pkg_cnt;
	int recv_pkg_cnt;

	void* user_data;
	struct event_base* base;
	struct bufferevent* bufvents;
	struct CRingBuff* recv_buf;
	NETTIMESTAP time_stamp;
	//!process state
	long long llLastBeatTime;
	BOOL is_connected;
};

typedef struct tag_channel_info CHANNELINFO;
typedef CHANNELINFO* LPCHANNELINFO;
typedef CHANNELINFO* LPWBUSCHANNEL;

struct tag_peer_info : public LINKBASE
{
	PEERID peer_id;

	int src_addr;   // net_addr_id(ip)
	int src_virtual_addr; //bus_addr

	int dst_addr;   //net_addr_id(ip)
	int dst_virtual_addr;

	int send_pkg_cnt;
	int recv_pkg_cnt;

	void* user_data;
	struct event_base* base;
	struct bufferevent* bufvents;
	struct CRingBuff* recv_buf;
	NETTIMESTAP time_stamp;
	//!process state
	long long llLastBeatTime;

};

typedef struct tag_peer_info PEERINFO;
typedef PEERINFO* LPPEERINFO;
typedef PEERINFO* LPWBUSPEER;

struct tag_wbus_global_val
{
	unsigned long long generate_channel_id;
};
typedef struct tag_wbus_global_val WBUSGLOBALVAL;
typedef struct tag_wbus_global_val* LPWBUSGLOBALVAL;

///////////////////////////////////////////////////////////////////////
//! frame head info define


/* ID */
typedef union
{
	uint32_t dwUINT32;							/* AGFRAMEHEAD_ID_UINT32  ,  uint32格式 */
	uint64_t ullUINUT64;						/* AGFRAMEHEAD_ID_UINT64  ,  uint64格式 */
	char szSTRING[FRAMEHEAD_MAX_ID_LENGTH];		/* AGFRAMEHEAD_ID_STRING  ,  字符串格式 */
}AGFRAMEHEADIDVALUE;

/* 用户账号 */
typedef struct
{
	uint16_t  wType;							/* 账号类型，0表示无账号信息，1表示QQ，4098表示微信OPENDID，等，是枚举 tagACCOUNT */
	uint8_t   hFormat;							/* 账号格式: Bind Macrogroup:AGFRAMEHEAD_ID */
	AGFRAMEHEADIDVALUE stValue;					/* 账号值 */
	uint64_t  ullUid;							/* Ver.19  经过TCONND账号映射的ID，为0表示无效 */
}AGFRAMEHEADACCOUNT;


typedef struct  
{
	int16_t nFamily;                            /* PF_INET 表示ipv4，PF_INET6表示ipv6 */
	uint16_t wPort;
	uint32_t ullp;
	uint32_t ipv6_padding[3];                  /* Ver.31 ipv6总共占用4个int ， 为了兼容老代码，ipv6分成 ip + ipv6_padding两部分 */
}AGIPINFO;

//! 鉴权相关
typedef struct  
{
	uint16_t    wAtkLen;							/* atk长度 */
	uint8_t		szAtk[FRAMEHEAD_MAX_SIG_LEN];       /* atk鉴权票据内容，注意:无\0结尾 */
	uint32_t	dwAtkExpireIn;
	uint16_t    wAuthExDataVer;
	uint16_t    wAuthExDataLen;
	uint8_t     szAuthExData[FRAMEHEAD_MAX_SIG_LEN];
}AGFRAMEAUTHAPS;

//! QQ签名相关
typedef struct  
{
	uint32_t	dwUin;
	int16_t		nSigVer;
	uint32_t	dwSigTime;
	int16_t		nSigLen;
	uint8_t		szSigBuff[FRAMEHEAD_MAX_SIG_LEN];
	uint32_t	dwAuthExDataLen;
	uint32_t	szAuthExData[FRAMEHEAD_MAX_SIG_LEN];
}AGFRAMEAUTHQQ;

typedef union 
{
	AGFRAMEAUTHAPS stAuthService ;/* Ver 17 */
	//AGFRAMEAUTHQQ  stAuthQQV1;  /*QQ*/
}AGFRAMEAUTHDATA;

typedef struct  
{
	AGFRAMEHEADACCOUNT	stUserAccount;
	AGFRAMEAUTHDATA		stAuthData;
	uint64_t			ullUid;
	uint64_t			ullReserved;
	int32_t				iAuthType;
	int32_t				iWaitTime;
	int32_t				iClientType;
	uint8_t				bRelayFlag;
	uint8_t				bIsUidNew;
	uint32_t			dwConnectExtInt;
	uint32_t			dwListenType;
	AGIPINFO			stSIPInfo;
}AGFRAMECMDSTART;

/* 连接断开包 */
typedef struct  
{
	int32_t iReason;					/* TCONND到GAMESVR的STOPREASON可以参考TFRAMEHAD_STOP_REASON的定义，GAMESVR下行包的STOPREASON可以自己定义，但注意不要和TCONND混淆 */
	AGFRAMEHEADACCOUNT stUserAccount;   /* Ver.22 stop消息的account只是TCONND上行会填，用于业务透传下下哦i，GAMESVR下行不需要填 */
}AGFRAMECMDSTOP;

/* 连接标识 */
typedef struct 
{
	int32_t		iID;
	int32_t		iConnIdx;
	uint64_t	ullExtend;
	uint64_t	ullCallBack;
}AGFRAMEIDENT;

/* 通信包 */
typedef struct 
{
	uint64_t ullUid;					/*账号*/
	int8_t   chValid;					/*是否组播： 0：单播  1：组播  2：广播 tagBROCAST_TYPE*/
	int8_t   chNoEnc;					/*下行包是否加密*/
	int8_t   chSendImmediately;			/* 在tconnd开启包含功能时，消息包是否立即发送 */
	int16_t  nCount;					/* 组播个数 */
	AGFRAMEIDENT astIdents[FRAMEHEAD_MAX_BATCH_IDENT];
}AGFRAMECMDINPROC;

typedef struct
{
	char szGroupName[GATEWAY_MULTICAST_GROUPNAME_LEN];
}AGFRAMECMDMULTICASTREMOVEIDENTREQ;

typedef struct
{
	char szGroupName[GATEWAY_MULTICAST_GROUPNAME_LEN];
}AGFRAMECMDMULTICASTADDIDENTREQ;

typedef struct
{
	char szGroupName[GATEWAY_MULTICAST_GROUPNAME_LEN];		/* 需要发消息的组播组名 */
	int8_t chNoEnc;											/* 下行包是否加密 */
	int8_t chSendImmediately;								/* 在tconnd开启包含合并功能时，消息包是否立即发送 */
}AGFRAMECMDMULTICASTDELIVERMSGREQ;


typedef struct
{
	int32_t wMsg;
	int32_t nMsgSrcAddr;
}INTERNALMESSAGEHEADER;

typedef struct
{
	uint32_t dwUid;
	int32_t  iRelayType;
	AGFRAMEIDENT stOld;
	AGFRAMEIDENT stNew;
	AGFRAMEHEADACCOUNT stUserAccount;
	AGIPINFO stSIPInfo;
	uint32_t dwConnectExtInt;
}AGFRAMECMDRELAY;


typedef struct
{
	uint64_t ullUid;				/* 账号 */
	AGIPINFO stSIPInfo;				/* ip port */
}AGFRAMECMDNOPTIFY;

typedef union 
{
	AGFRAMECMDSTART		stStart;
	AGFRAMECMDSTOP		stStop;
	AGFRAMECMDINPROC	stInProc;
	AGFRAMECMDNOPTIFY	stNotify;
	AGFRAMECMDMULTICASTADDIDENTREQ		stMulticastAddIdenReq;
	AGFRAMECMDMULTICASTDELIVERMSGREQ	stMulticastDeliverMsgReq;
	AGFRAMECMDMULTICASTREMOVEIDENTREQ	stMulticastRemoveIdenReq;
	AGFRAMECMDRELAY		stRelay;
}AGFRAMECMDDATA;

typedef struct tagAGFrameHead
{
	int8_t			chCmd;				/* 协议类型Bind Marcogroup:AGFRAMEHEAD_CMD_ID */
	int32_t			iID;				/* 会话ID，由逻辑服务器在start或者relay会包中设置，对于组播包无意义，-1表示后端服务器无绘画，否则表示有会话 */
	int32_t			iConnIdx;			/* 连接标识，对逻辑服务器只读不能修改，对于组播包忽略该字段 */
	int64_t			stTimeStamp;		/* 时间戳信息 */
	AGIPINFO		ipInfo;				/* 连接IP信息 */
	AGFRAMECMDDATA	stCmdData;			/* 协议数据 */
}AGFRAMEHEAD;

typedef struct
{
	int virtual_addr;
	int net_ip;
	int port;
}AGVIRTUALADDR;


typedef struct tagAGBusPairNetAddr
{
	AGVIRTUALADDR bus1;
	AGVIRTUALADDR bus2;
	int recv_buf_size;
	int send_bus_size;
	BOOL connected;
	CHANNELID channel_id;
}AGBUSPAIRNETADDR;

typedef struct
{
	int msg_size;
	char msg[0];
}AGBUSBASEHEAD;


typedef struct tagPeerHead
{
	PEERID peer_id;
	AGBUSBASEHEAD cl_head;
}AGPEERHEAD;


typedef struct tagChnnelHead
{
	CHANNELID channel_id;
	AGBUSBASEHEAD sv_head;
}AGCHANNELHEAD;


typedef struct
{
	struct  
	{
		WBUSADDR	local_ip;
		uint16_t	local_port;
		int			reconnect_interval;
		int			reconnect_timer;
	}bus_addr;

	struct
	{
		PEERID		peer_id;
		uint16_t	peer_port;
		int			recv_buff_size;
	}peer_addr;

}AGBUSLOCALINFO;


typedef struct
{
	WBUSADDR	virtual_ip;
	int port;
}CHANNEL_CONNECT_EVNT;

typedef struct
{
	int ip;
	int port;
	PEERID peer_id;
}PEER_CONNECT_EVNT;

typedef struct
{
	union
	{
		CHANNEL_CONNECT_EVNT cce;
		PEER_CONNECT_EVNT  pce;
	};
}NETWORKEVENT;


#pragma pack(pop)


#define BUS_HANDLE_NAME "bus_handle"
#define BUS_RECV_NAME "bus_recv"
#define BUS_CHANEL_NAME "bus_channel"
#define CHANEL_INFO_NAME "channel_info"
#define PEER_INFO_NAME  "pee_info"
#define MSG_QUEUE_NAME "msg_queue"
#define PAIR_SERVER_NAME "bus_pair_server_name"
#define COMMON_NAME_LEN              (20)  // ! temporaty
//#define "define/string_def.h"

template<class T>
const char* get_vm_name(int n)
{
	static char szDataName[COMMON_NAME_LEN] = { 0 };
	snprintf(szDataName, COMMON_NAME_LEN, "%s_%d", typeid(T).name(), n);
	return szDataName;
}

#define MAKE_WBUS_CHANNEL_ID(ip,port) ((uint64_t)ip<<32|port)

#endif //_WBUS_DEF_H__



























