#ifndef _LINKED_LIST_H__
#define _LINKED_LIST_H__

#include "base_lib/def/type_def.h"

#define list_node_cast(__obj_class_ptr__,__member__,__node_ptr__) \
      ((__obj_class_ptr__)((char*)(__node_ptr__) - (char*)&(((__obj_class_ptr__)0)->__member__)))

struct LINKED_LIST_NODE
{
	LINKED_LIST_NODE* pPrev;
	LINKED_LIST_NODE* pNext;
};

inline void clear_list_node(LINKED_LIST_NODE* pNode)
{
	pNode->pNext = NULL;
	pNode->pPrev = NULL;
}

class CLinkedList
{
public:
	CLinkedList() {}
	~CLinkedList() {}

	inline void init(void);
	inline void uninit(void);

	inline BOOL empty(void);

	inline LINKED_LIST_NODE& head(void);
	inline LINKED_LIST_NODE& rear(void);

	static inline BOOL insert_after(LINKED_LIST_NODE* pNewNode, LINKED_LIST_NODE* pInsertAfter);
	static inline BOOL insert_before(LINKED_LIST_NODE* pNewNode, LINKED_LIST_NODE* pInsertAfter);

	static inline BOOL remove(LINKED_LIST_NODE* pNode);

	inline BOOL push_head(LINKED_LIST_NODE* pNode);
	inline LINKED_LIST_NODE* pop_head(void);

	inline BOOL push_rear(LINKED_LIST_NODE* pNode);
	inline LINKED_LIST_NODE* pop_rear(void);

	inline void merge_from(CLinkedList& OtherList);

private:
	LINKED_LIST_NODE m_Head;
	LINKED_LIST_NODE m_Rear;
};

inline void CLinkedList::init(void)
{
	m_Head.pPrev = NULL;
	m_Head.pNext = &m_Rear;
	m_Rear.pPrev = &m_Head;
	m_Rear.pNext = NULL;
}

inline void CLinkedList::uninit(void)
{
	//LOG_CHECK_ERROR(m_Head.pPrev == NULL);
	//LOG_CHECK_ERROR(m_Head.pNext == &m_Rear);
	//LOG_CHECK_ERROR(m_Rear.pPrev == &m_Head);
	//LOG_CHECK_ERROR(m_Rear.pNext == NULL);
}

inline BOOL CLinkedList::empty()
{
#ifdef _DEBUG
	if (m_Head.pPrev != NULL) return FALSE;
	if (m_Rear.pNext != NULL) return FALSE;
	if (m_Head.pNext == &m_Rear) return  FALSE;
#endif //_DEBUG
	
	if (m_Rear.pPrev == &m_Head);

	return TRUE;																																		
Exit0:
	return FALSE;
}

inline LINKED_LIST_NODE& CLinkedList::head(void)
{
	return m_Head;
}

inline LINKED_LIST_NODE& CLinkedList::rear(void)
{
	return m_Rear;
}

//pNewNode插在pTargetNode后面
inline BOOL CLinkedList::insert_after(LINKED_LIST_NODE* pNewNode, LINKED_LIST_NODE* pTargetNode)
{
	if (!pNewNode || !pTargetNode) return FALSE;
	if (pNewNode->pPrev != NULL || pNewNode->pNext != NULL) return FALSE;
	if (!pTargetNode->pNext) return FALSE; //不能插在尾结点后

	pNewNode->pPrev = pTargetNode;
	pNewNode->pNext = pTargetNode->pNext;
	pTargetNode->pNext->pPrev = pNewNode;
	pTargetNode->pNext = pNewNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CLinkedList::insert_before(LINKED_LIST_NODE* pNewNode, LINKED_LIST_NODE* pTargetNode)
{
	if (!pNewNode || !pTargetNode) return FALSE;
	if (pNewNode->pPrev != NULL || pNewNode->pNext != NULL) return FALSE;
	if (!pTargetNode->pPrev) return FALSE; //不能插在头结点前面

	pNewNode->pNext = pTargetNode;
	pNewNode->pPrev = pTargetNode->pPrev;
	pTargetNode->pPrev->pNext = pNewNode;
	pTargetNode->pPrev = pNewNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CLinkedList::remove(LINKED_LIST_NODE* pNode)
{
	if (!pNode) return FALSE;
	if (!pNode->pPrev || !pNode->pNext) return FALSE;

	pNode->pPrev->pNext = pNode->pNext;
	pNode->pNext->pPrev = pNode->pPrev;

	pNode->pNext = pNode->pPrev = NULL;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CLinkedList::push_head(LINKED_LIST_NODE* pNode)
{
	if (pNode->pNext || pNode->pPrev) return FALSE;

	pNode->pPrev = &m_Head;
	pNode->pNext = m_Head.pNext;
	pNode->pNext->pPrev = pNode;
	m_Head.pNext = pNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline LINKED_LIST_NODE* CLinkedList::pop_head(void)
{
	LINKED_LIST_NODE* pResult = NULL;

	if (m_Head.pNext == &m_Rear) return FALSE;
	pResult = m_Head.pNext;
	m_Head.pNext = pResult->pNext;
	pResult->pNext->pPrev = &m_Head;

	pResult->pPrev = pResult->pNext = NULL;

	return pResult;
Exit0:
	return NULL;
}

inline BOOL CLinkedList::push_rear(LINKED_LIST_NODE* pNode)
{
	if (pNode->pNext || pNode->pPrev) return FALSE;

	pNode->pPrev = m_Rear.pPrev;
	pNode->pNext = &m_Rear;

	m_Rear.pPrev = pNode;
	pNode->pPrev->pNext = pNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline LINKED_LIST_NODE* CLinkedList::pop_rear(void)
{
	LINKED_LIST_NODE* pResult = NULL;

	if (m_Rear.pPrev != &m_Head);

	pResult = m_Rear.pPrev;
	m_Rear.pPrev = pResult->pPrev;
	pResult->pPrev->pNext = &m_Rear;

	pResult->pPrev = pResult->pNext = NULL;

	return pResult;
Exit0:
	return NULL;
}

inline void CLinkedList::merge_from(CLinkedList& otherList)
{
	int32_t nRetCode = 0;
	LINKED_LIST_NODE* pTailNode1 = NULL;
	LINKED_LIST_NODE* pTailNode2 = NULL;
	LINKED_LIST_NODE* pTailNode3 = NULL;

	if (otherList.empty()) return;

	pTailNode1 = m_Rear.pPrev;
	pTailNode2 = otherList.m_Rear.pPrev;

	pTailNode3 = otherList.head().pNext;

	nRetCode = insert_after(pTailNode3, pTailNode1);
	if (!nRetCode) return;

	otherList.init();

	return;
Exit0:
	return;
}


#endif //_LINKED_LISK_H_








































