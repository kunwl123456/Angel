#ifndef ANGEL_SHM_REGISTRY_H_
#define ANGEL_SHM_REGISTRY_H_

#include <map>
#include <string>
#include <vector>

#include "base_lib/def/type_def.h"

typedef void* (*ANGEL_SHM_CREATE_FN)();
typedef void* (*ANGEL_SHM_RESUME_FN)(void* pMemory);
typedef void (*ANGEL_SHM_DESTROY_FN)(void* pObject);

struct ANGEL_SHM_OBJECT_DESC
{
	ANGEL_SHM_OBJECT_DESC();

	int32_t typeId;
	std::string className;
	size_t objectSize;
	int32_t itemCount;
	BOOL useHash;
	BOOL singleton;
	ANGEL_SHM_CREATE_FN createFn;
	ANGEL_SHM_RESUME_FN resumeFn;
	ANGEL_SHM_DESTROY_FN destroyFn;
};

class CAngelShmRegistry
{
public:
	static CAngelShmRegistry& Instance();

	BOOL Register(const ANGEL_SHM_OBJECT_DESC& desc);
	const ANGEL_SHM_OBJECT_DESC* Find(int32_t typeId) const;
	const ANGEL_SHM_OBJECT_DESC* Find(const std::string& className) const;
	const std::vector<ANGEL_SHM_OBJECT_DESC>& GetAll() const { return m_Objects; }
	BOOL Validate() const;
	void Clear();

private:
	CAngelShmRegistry() {}
	CAngelShmRegistry(const CAngelShmRegistry&);
	CAngelShmRegistry& operator=(const CAngelShmRegistry&);

private:
	std::vector<ANGEL_SHM_OBJECT_DESC> m_Objects;
	std::map<int32_t, size_t> m_TypeIndex;
	std::map<std::string, size_t> m_NameIndex;
};

template <typename T>
void* AngelShmDefaultCreate()
{
	return new T();
}

template <typename T>
void* AngelShmDefaultResume(void* pMemory)
{
	return pMemory ? pMemory : new T();
}

template <typename T>
void AngelShmDefaultDestroy(void* pObject)
{
	delete static_cast<T*>(pObject);
}

template <typename T>
BOOL AngelRegisterShmObject(int32_t typeId, const char* className, int32_t itemCount, BOOL useHash, BOOL singleton)
{
	ANGEL_SHM_OBJECT_DESC desc;
	desc.typeId = typeId;
	desc.className = className ? className : typeid(T).name();
	desc.objectSize = sizeof(T);
	desc.itemCount = itemCount;
	desc.useHash = useHash;
	desc.singleton = singleton;
	desc.createFn = &AngelShmDefaultCreate<T>;
	desc.resumeFn = &AngelShmDefaultResume<T>;
	desc.destroyFn = &AngelShmDefaultDestroy<T>;
	return CAngelShmRegistry::Instance().Register(desc);
}

#endif // ANGEL_SHM_REGISTRY_H_
