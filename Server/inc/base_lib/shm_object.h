#ifndef _SHM_OBJECT_H_
#define _SHM_OBJECT_H_

#include <errno.h>
#include <typeinfo>

#include "base_lib/shm.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4312)
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#define OBJECT_DATA_POOL_TAG  (0xFEEDBACB)

template<class T>
class CShmObject
{
public:
	CShmObject();
	~CShmObject();

	BOOL init(const char* pcszInstanceTag);
	BOOL uninit(void);

	T* get_data(void);
	T* restore_data(void);
	T* create_data(void);

	BOOL destroy_data(void);

private: 
	void _reset();

private:
	char m_szPoolName[POOL_NAME_LEN];
	int32_t m_nHandle;

	struct SHM_DATA
	{
		uint32_t   m_dwTag;
		uint32_t   m_dwThisSize;
		void*      m_pAddr;
		T          m_stData;
	};
	SHM_DATA*      m_pData;

};

template<class T>
CShmObject<T>::CShmObject()
{
	m_szPoolName[0] = 0;
	m_nHandle = -1;
	m_pData = NULL;
}

template<class T>
CShmObject<T>::~CShmObject()
{

}

template<class T>
void CShmObject<T>::_reset()
{
	m_nHandle = -1;
	m_pData = NULL;
}

template<class T>
T* CShmObject<T>::get_data()
{
	if (!m_pData) return FALSE;
	return &m_pData->m_stData;
Exit0:
	return NULL;
}

template<class T>
T* CShmObject<T>::create_data()
{
	int32_t nRetCode = 0;



	return NULL;
}

template<class T>
T* CShmObject<T>::restore_data()
{
	int32_t nRetCode = 0;
	return NULL;
}

template<class T>
BOOL CShmObject<T>::destroy_data()
{
#if defined(WIN32) | defined(WIN64)
	_reset();
	return TRUE;
#else
	return TRUE;
#endif
}

template<class T>
BOOL CShmObject<T>::init(const char* pcszInstanceTag)
{
	int32_t nRetCode = 0;

	if (pcszInstanceTag)
	{
		snprintf(m_szPoolName, POOL_NAME_LEN, "%s_%s", typeid(T).name(), pcszInstanceTag);
	}
	else
	{
		snprintf(m_szPoolName, POOL_NAME_LEN, "%s_null", typeid(T).name());
	}

	_reset();

	return TRUE;
Exit0:
	uninit();
	return FALSE;
}

template<class T>
BOOL CShmObject<T>::uninit(void)
{
	destroy_data();
	return TRUE;
}
#ifdef WIN32
#pragma warning(pop)
#endif //WIN32


#endif //_SHM_OBJECT_H_
























