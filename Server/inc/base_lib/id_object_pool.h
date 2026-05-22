#ifndef _ID_OBJECT_POLL_OLD_H_
#define _ID_OBJECT_POLL_OLD_H_

#include <errno.h>
#include <typeinfo>
#include <vector>
#include <map>
#include "rb_tree.h"

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
	std::map<uint64_t, T*> m_HeapObjects;
	typename std::map<uint64_t, T*>::iterator m_HeapIterator;

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

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::get_first_obj(void)
{
	m_HeapIterator = m_HeapObjects.begin();
	return m_HeapIterator == m_HeapObjects.end() ? NULL : m_HeapIterator->second;
}

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::get_next_obj(void)
{
	if (m_HeapIterator == m_HeapObjects.end())
	{
		return NULL;
	}
	++m_HeapIterator;
	return m_HeapIterator == m_HeapObjects.end() ? NULL : m_HeapIterator->second;
}

template<class T, uint32_t ALIGN>
CIDObjectPool<T, ALIGN>::CIDObjectPool(void)
{
	m_szPoolName[0] = 0;
	m_pChunkHead = NULL;
	m_HeapIterator = m_HeapObjects.end();
}

template<class T, uint32_t ALIGN>
CIDObjectPool<T, ALIGN>::~CIDObjectPool(void)
{
	clear();
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::init(int32_t nInitSize, const char* pcszInstanceTag)
{
	(void)nInitSize;
	snprintf(m_szPoolName, ID_POOL_NAME_LEN, "%s", pcszInstanceTag ? pcszInstanceTag : "default");
	return TRUE;
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::restore(const char* szInstanceTag)
{
	return init(0, szInstanceTag);
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::uninit(void)
{
	return clear();
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::clear(void)
{
	for (typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.begin(); it != m_HeapObjects.end(); ++it)
	{
		delete it->second;
	}
	m_HeapObjects.clear();
	m_HeapIterator = m_HeapObjects.end();
	return TRUE;
}

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::new_object(uint64_t ID)
{
	T* pObject = find(ID);
	if (pObject)
	{
		return pObject;
	}
	pObject = new T();
	m_HeapObjects[ID] = pObject;
	return pObject;
}

template<class T, uint32_t ALIGN>
BOOL CIDObjectPool<T, ALIGN>::delete_object(T* pObject)
{
	if (!pObject)
	{
		return FALSE;
	}
	for (typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.begin(); it != m_HeapObjects.end(); ++it)
	{
		if (it->second == pObject)
		{
			delete it->second;
			m_HeapObjects.erase(it);
			m_HeapIterator = m_HeapObjects.end();
			return TRUE;
		}
	}
	return FALSE;
}

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::find(uint64_t ID)
{
	typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.find(ID);
	return it == m_HeapObjects.end() ? NULL : it->second;
}

template<class T, uint32_t ALIGN>
T* CIDObjectPool<T, ALIGN>::get(uint64_t index)
{
	uint64_t i = 0;
	for (typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.begin(); it != m_HeapObjects.end(); ++it, ++i)
	{
		if (i == index)
		{
			return it->second;
		}
	}
	return NULL;
}

template<class T, uint32_t ALIGN>
template<class TFunc>
inline BOOL CIDObjectPool<T, ALIGN>::traverse(TFunc& rFunc)
{
	for (typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.begin(); it != m_HeapObjects.end(); ++it)
	{
		if (!rFunc(it->second))
		{
			return FALSE;
		}
	}
	return TRUE;
}

template<class T, uint32_t ALIGN>
inline int32_t CIDObjectPool<T, ALIGN>::get_count()
{
	return static_cast<int32_t>(m_HeapObjects.size());
}

template<class T, uint32_t ALIGN>
inline int32_t CIDObjectPool<T, ALIGN>::get_size(void)
{
	return get_count();
}

template<class T, uint32_t ALIGN>
inline int32_t CIDObjectPool<T, ALIGN>::get_free_size(void)
{
	return 0;
}

template<class T, uint32_t ALIGN>
inline int32_t CIDObjectPool<T, ALIGN>::get_used_size(void)
{
	return get_count();
}

template<class T, uint32_t ALIGN>
inline int32_t CIDObjectPool<T, ALIGN>::get_obj_size(void)
{
	return static_cast<int32_t>(sizeof(T));
}

template<class T, uint32_t ALIGN>
void* CIDObjectPool<T, ALIGN>::get_obj_id(T* pObject)
{
	for (typename std::map<uint64_t, T*>::iterator it = m_HeapObjects.begin(); it != m_HeapObjects.end(); ++it)
	{
		if (it->second == pObject)
		{
			return reinterpret_cast<void*>(static_cast<uintptr_t>(it->first));
		}
	}
	return NULL;
}

template<class T, uint32_t ALIGN>
void CIDObjectPool<T, ALIGN>::check_fence(void)
{
}

#endif
