#ifndef _ID_OBJECT_POLL_OLD_H_
#define _ID_OBJECT_POLL_OLD_H_

#include <errno.h>
#include <typeinfo>
#include <vector>
#include "rb_tree.h"
#include "shm.h"

#ifndef _DEBUG
#define _DEBUG
#endif

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4312)
#endif //WIN32

#define FENCE_NUMBER            (0xABCDEF0123456789LL)
#define ID_POOL_MAGIC_FREE_TAG  (0xFEEDBAC9)
#define ID_POOL_HEAD_MAGIC      (0xDEADBAC9)
#define HASH_TABLE_SIZE         (1024 * 16)
#define ID_POOL_NAME_LEN        (128)
#define MAX_CHUNK_COUNT         (64)

#if defined(_DEBUG)
#define _DEBUG_FENCE
#endif // _DEBUG

template<class T, uint32_t ALIGN = 8>
class CIDObjectPool
{
public:
	CIDObjectPool(void);
	~CIDObjectPool(void);

	BOOL init(int32_t nInitSize, const char* pcszInstanceTag = NULL);
	BOOL restore(const char* szInstanceTag);
	BOOL uninit(void);
	BOOL clear(void);

	T* new_object(uint64_t ID);
	BOOL delete_object(T* pObject);
	T* find(uint64_t ID);
	T* get(uint64_t index);

	template<class TFunc>
	inline BOOL traverse(TFunc& rFunc);
	inline int32_t get_count();

	T* get_first_obj(void);
	T* get_next_obj(void);

	inline int32_t get_size(void);
	inline int32_t get_free_size(void);
	inline int32_t get_used_size(void);
	inline int32_t get_obj_size(void);

	void* get_obj_id(T* pObject);

	void check_fence(void);

private:
	char m_szPoolName[ID_POOL_NAME_LEN];

	struct T_NODE
	{
		T Data;
#ifdef _DEBUG_FENCE
		uint64_t   qwFence;
		char       Padding[((sizeof(T) + sizeof(uint64_t) + (ALIGN - 1))& (~(ALIGN - 1))) - (sizeof(T) + sizeof(uint64_t))];
#else
		char       Padding[((sizeof(T) + (ALIGN - 1)) &(~(ALIGN - 1))) - sizeof(T)];
#endif
	};

	struct CHUNK_NODE
	{
		uint32_t        dwTag;
		int32_t         nIndex;
		RB_TREE_NODE*   pRbNode;
		T_NODE*         pTNode;
		int32_t         nSize;
		CHUNK_NODE*     pNext;
	};

	struct CHUNK_HEAD
	{
		uint32_t        m_dwMagic;
		uint32_t        m_dwPadding;
		int32_t         m_nSize;
		int32_t         m_nChunkCount;
		RB_TREE_NODE    m_DataMapRoot[HASH_TABLE_SIZE];
		RB_TREE_NODE    m_FreeListHead;
		RB_TREE_NODE    m_FreeListRear;
		int32_t         m_nFreeListSize;
		int32_t         m_nObjSize;
		int32_t         m_nInitSize;
		int32_t         m_nCurMapRootIdx;
		RB_TREE_NODE*   m_pCurTreeNode;
		CHUNK_NODE*     m_pChunkNode;
		void*   m_pThisAddr;
		void*   m_pChunkAddr[MAX_CHUNK_COUNT];
	};

	CHUNK_HEAD* m_pChunkHead;

	BOOL _create_chunk_head(void);
	BOOL _restore_chunk_head(void);
	BOOL _destroy_chunk_head(void);

	BOOL _create_chunk(int32_t nSize);
	BOOL _find_chunk(int32_t nChunkIndex);
	BOOL _restore_chunk(int32_t nCHunkIndex);
	BOOL _destroy_chunk(int32_t nChunkIndex);
	RB_TREE_NODE* _get_data_node(T* pData);
	T* _get_data(RB_TREE_NODE* pNode);

	inline void _push_into_free_list(RB_TREE_NODE* pDataNode);
	inline RB_TREE_NODE*  _pop_from_free_list(void);
	inline BOOL _is_free_list_empty(void);
};

/*
template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::get_first_obj(void)
{
	if (!m_pChunkHead) goto Exit0;
	//LOG_PROCESS_ERROR(m_pChunkHead);

	while (m_pChunkHead->m_nCurMapRootIdx < HASH_TABLE_SIZE)
	{
		if (m_pChunkHead->m_DataMapRoot[m_pChunkHead->m_nCurMapRootIdx].pRootNode)
		{
			m_pChunkHead->m_pCurTreeNode = rb_first(&m_pChunkHead->m_DataMapRoot[m_pChunkHead->m_nCurMapRootIdx]);
			return _get_data(m_pChunkHead->m_pCurTreeNode);
		}
		++m_pChunkHead->m_nCurMapRootIdx;
	}

Exit0:
	return NULL;
}

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::get_next_obj(void)
{
	RB_TREE_NODE* pNextNode = NULL;
	if (!m_pChunkHead) goto Exit0;
	//LOG_PROCESS_ERROR(m_pChunkHead);

	pNextNode = rb_next(m_pChunkHead->m_pCurTreeNode);

	while (!pNextNode && ++m_pChunkHead->m_nCurMapRootIdx < HASH_TABLE_SIZE)
	{
		if (m_pChunkHead->m_DataMapRoot[m_pChunkHead->m_nCurMapRootIdx].pRootNode)
		{
			m_pChunkHead->m_pCurTreeNode = rb_first(&m_pChunkHead->m_DataMapRoot[m_pChunkHead->m_nCurMapRootIdx]);
			break;
		}
	}

	m_pChunkHead->m_pCurTreeNode = pNextNode;

	return _get_data(m_pChunkHead->m_pCurTreeNode);

Exit0:
	return NULL;
}

template<class T, uint32_t ALIGN>
CIDObjectPool<T, ALIGN>::CIDObjectPool(void)
{
	m_szPoolName[0] = 0;
	m_pChunkHead = NULL;
}

template<class T, uint32_t ALIGN>
CIDObjectPool<T, ALIGN>::~CIDObjectPool(void)
{

}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::_create_chunk_head(void)
{
	int32_t nResult = -1;
	int32_t nRetCode = 0;
	char szChunkHeadName[ID_POOL_NAME_LEN] = {};

	//alloc memory
	snprintf(szChunkHeadName, ID_POOL_NAME_LEN, "CH_%s", m_szPoolName);
	m_pChunkHead = (CHUNK_HEAD*)vm_new_chunk(szChunkHeadName, sizeof(CHUNK_HEAD));

	//LOG_PROCESS_ERROR(m_pChunkHead);
	if (!m_pChunkHead) return FALSE;

	memset(m_pChunkHead, 0, sizeof(CHUNK_HEAD));
	m_pChunkHead->m_dwMagic = ID_POOL_HEAD_MAGIC;
	m_pChunkHead->m_pThisAddr = m_pChunkHead;

	m_pChunkHead->m_FreeListHead.pLeft = NULL;
	m_pChunkHead->m_FreeListHead.pRight = &m_pChunkHead->m_FreeListRear;
	m_pChunkHead->m_FreeListRear.pLeft = &m_pChunkHead->m_FreeListHead;
	m_pChunkHead->m_FreeListRear.pRight = NULL;

	return TRUE;
Exit0:
	return FALSE;
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::_restore_chunk_head(void)
{
	int32_t nRetCode = 0;
	BOOL bOverlap = FALSE;
	char szChunkHeadName[ID_POOL_NAME_LEN] = {};

	return TRUE;
Exit0:
	return FALSE;
}
*/

#endif
