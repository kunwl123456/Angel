#ifndef _BT_DEF_H__
#define _BT_DEF_H__


#define MAX_BT_CHILD_COUNT             (64)
#define MAX_CALL_FRAME                 (16)
#define MAX_BT_PARAM                   (6)
#define MAX_NODE_NAME_LEN              (32)


enum BT_RETURN_VALUE
{
	btvError = 100,

	brvFail,
	brvSuccess,

	brvTotal
};


enum BT_NODE_TYPE
{
	btInvalid = 0,

	btSelector,
	btSequencer,
	btParallel,
	btNot,
	btDebug,
	btRollback,
	btRandom,
	btRepeat,

	btUserDefine,

	btTotal = 1024
};

struct BT_NODE
{
	unsigned char  uChildrenCount;
	unsigned char  uReturnValue;
	unsigned short uType;
	unsigned int   uVarType;

	int32_t  nParam[MAX_BT_PARAM];

	struct BT_NODE* pChild;
	struct BT_NODE* pSibling;

};

struct BT_TREE;

#pragma pack(push,1)
struct BT_CTX
{
	struct CALL_FRAME
	{
		BT_NODE* pNode;
		BT_NODE* pChild;
	};
	CALL_FRAME  CallFrame[MAX_CALL_FRAME];

	void*     pOwner;
	uint64_t  qwOwnerID;
	//
	uint64_t  qwScriptNameHash;
	void*     pBTEventTemplate;
	uint8_t   byReturnValue;
};

#pragma pack(pop)


#endif //_BT_DEF_H__
















