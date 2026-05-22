#ifndef ANGEL_SHM_OBJECT_H_
#define ANGEL_SHM_OBJECT_H_

#include <new>
#include <stdio.h>
#include <string.h>
#include <typeinfo>

#include "base_lib/def/type_def.h"
#include "base_lib/nf_shm/angel_shm_module.h"

#define ANGEL_SHM_OBJECT_TAG 0xA6E15A01
#define ANGEL_SHM_POOL_NAME_LEN 128

template<class T>
class CAngelShmObject
{
public:
	CAngelShmObject()
		: m_pData(NULL)
	{
		m_szPoolName[0] = 0;
	}

	~CAngelShmObject()
	{
	}

	BOOL init(const char* pcszInstanceTag)
	{
		if (pcszInstanceTag)
		{
			snprintf(m_szPoolName, ANGEL_SHM_POOL_NAME_LEN, "%s_%s", typeid(T).name(), pcszInstanceTag);
		}
		else
		{
			snprintf(m_szPoolName, ANGEL_SHM_POOL_NAME_LEN, "%s_null", typeid(T).name());
		}
		m_pData = NULL;
		return TRUE;
	}

	BOOL uninit()
	{
		if (m_szPoolName[0])
		{
			CAngelShmModule::Instance().CloseSegment(m_szPoolName);
		}
		m_pData = NULL;
		return TRUE;
	}

	T* get_data()
	{
		return m_pData ? &m_pData->m_stData : NULL;
	}

	T* create_data()
	{
		BOOL newlyCreated = FALSE;
		void* pMem = CAngelShmModule::Instance().CreateSegment(m_szPoolName, sizeof(SHM_DATA), &newlyCreated);
		if (!pMem)
		{
			return NULL;
		}

		m_pData = static_cast<SHM_DATA*>(pMem);
		if (newlyCreated || m_pData->m_dwTag != ANGEL_SHM_OBJECT_TAG || m_pData->m_dwThisSize != sizeof(SHM_DATA))
		{
			memset(m_pData, 0, sizeof(SHM_DATA));
			m_pData->m_dwTag = ANGEL_SHM_OBJECT_TAG;
			m_pData->m_dwThisSize = sizeof(SHM_DATA);
			m_pData->m_pAddr = m_pData;
			new (&m_pData->m_stData) T();
		}
		CAngelShmModule::Instance().SetCreateMode(ANGEL_OBJ_MODE_INIT);
		return &m_pData->m_stData;
	}

	T* restore_data()
	{
		void* pMem = CAngelShmModule::Instance().OpenSegment(m_szPoolName, sizeof(SHM_DATA));
		if (!pMem)
		{
			return NULL;
		}

		m_pData = static_cast<SHM_DATA*>(pMem);
		if (m_pData->m_dwTag != ANGEL_SHM_OBJECT_TAG || m_pData->m_dwThisSize != sizeof(SHM_DATA))
		{
			m_pData = NULL;
			return NULL;
		}
		CAngelShmModule::Instance().SetCreateMode(ANGEL_OBJ_MODE_RESUME);
		return &m_pData->m_stData;
	}

	BOOL destroy_data()
	{
		if (m_pData)
		{
			m_pData->m_stData.~T();
		}
		m_pData = NULL;
		return CAngelShmModule::Instance().DestroySegment(m_szPoolName);
	}

private:
	struct SHM_DATA
	{
		uint32_t m_dwTag;
		uint32_t m_dwThisSize;
		void* m_pAddr;
		T m_stData;
	};

private:
	char m_szPoolName[ANGEL_SHM_POOL_NAME_LEN];
	SHM_DATA* m_pData;
};

template<class T>
class CShmObject : public CAngelShmObject<T>
{
};

#endif // ANGEL_SHM_OBJECT_H_
