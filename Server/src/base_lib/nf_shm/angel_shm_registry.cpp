#include "stdafx.h"

#include "base_lib/nf_shm/angel_shm_registry.h"

ANGEL_SHM_OBJECT_DESC::ANGEL_SHM_OBJECT_DESC()
	: typeId(0),
	  objectSize(0),
	  itemCount(0),
	  useHash(FALSE),
	  singleton(FALSE),
	  createFn(NULL),
	  resumeFn(NULL),
	  destroyFn(NULL)
{
}

CAngelShmRegistry& CAngelShmRegistry::Instance()
{
	static CAngelShmRegistry s_Registry;
	return s_Registry;
}

BOOL CAngelShmRegistry::Register(const ANGEL_SHM_OBJECT_DESC& desc)
{
	if (desc.typeId <= 0 || desc.className.empty() || desc.objectSize == 0 || desc.itemCount < 0)
	{
		return FALSE;
	}
	if (m_TypeIndex.find(desc.typeId) != m_TypeIndex.end())
	{
		return FALSE;
	}
	if (m_NameIndex.find(desc.className) != m_NameIndex.end())
	{
		return FALSE;
	}

	const size_t index = m_Objects.size();
	m_Objects.push_back(desc);
	m_TypeIndex[desc.typeId] = index;
	m_NameIndex[desc.className] = index;
	return TRUE;
}

const ANGEL_SHM_OBJECT_DESC* CAngelShmRegistry::Find(int32_t typeId) const
{
	std::map<int32_t, size_t>::const_iterator it = m_TypeIndex.find(typeId);
	return it == m_TypeIndex.end() ? NULL : &m_Objects[it->second];
}

const ANGEL_SHM_OBJECT_DESC* CAngelShmRegistry::Find(const std::string& className) const
{
	std::map<std::string, size_t>::const_iterator it = m_NameIndex.find(className);
	return it == m_NameIndex.end() ? NULL : &m_Objects[it->second];
}

BOOL CAngelShmRegistry::Validate() const
{
	for (size_t i = 0; i < m_Objects.size(); ++i)
	{
		const ANGEL_SHM_OBJECT_DESC& desc = m_Objects[i];
		if (desc.typeId <= 0 || desc.className.empty() || desc.objectSize == 0 || desc.itemCount < 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CAngelShmRegistry::Clear()
{
	m_Objects.clear();
	m_TypeIndex.clear();
	m_NameIndex.clear();
}
