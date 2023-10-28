#ifndef _RB_TREE_H__
#define _RB_TREE_H__

#include "def/type_def.h"

#pragma pack(push,1)
struct RB_TREE_NODE
{
	void* ID;
	union
	{
		RB_TREE_NODE* pNext;
		uint64_t uColor;
	};
	RB_TREE_NODE* pLeft;
	RB_TREE_NODE* pRight;
};
#pragma pack(pop)

typedef int32_t(*RB_TREE_COMPARE_FUNC_t)(void* pID1, void* pID2);

struct RB_TREE_ROOT
{
	RB_TREE_ROOT(void)
	{
		pRootNode = NULL;
		pCompareFunc = NULL;
	}
	RB_TREE_NODE* pRootNode;
	RB_TREE_COMPARE_FUNC_t pCompareFunc;
};

BOOL rb_insert(RB_TREE_NODE* pDataNode, RB_TREE_ROOT* pTreeRoot);
BOOL rb_erase(RB_TREE_NODE* pDataNode, RB_TREE_ROOT* pTreeRoot);

RB_TREE_NODE* rb_search(void* ID, RB_TREE_ROOT* pRootNode);
RB_TREE_NODE* rb_first(RB_TREE_ROOT* pRootNode);
RB_TREE_NODE* rb_last(RB_TREE_ROOT* pRootNode);

RB_TREE_NODE* rb_next(RB_TREE_ROOT* pRootNode);
RB_TREE_NODE* rb_prev(RB_TREE_ROOT* pRootNode);


#endif