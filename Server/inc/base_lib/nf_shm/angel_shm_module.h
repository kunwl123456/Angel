#ifndef ANGEL_SHM_MODULE_H_
#define ANGEL_SHM_MODULE_H_

#include <map>
#include <string>

#include "base_lib/def/type_def.h"
#include "base_lib/nf_shm/angel_plugin_runtime.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

enum ANGEL_OBJ_MODE
{
	ANGEL_OBJ_MODE_INIT = 0,
	ANGEL_OBJ_MODE_RESUME = 1,
	ANGEL_OBJ_MODE_RUNNING = 2
};

class CAngelShmModule
{
public:
	static CAngelShmModule& Instance();

	BOOL Init();
	BOOL Shut();

	void SetInitMode(ANGEL_OBJ_MODE mode) { m_InitMode = mode; }
	ANGEL_OBJ_MODE GetInitMode() const { return m_InitMode; }

	void SetCreateMode(ANGEL_OBJ_MODE mode) { m_CreateMode = mode; }
	ANGEL_OBJ_MODE GetCreateMode() const { return m_CreateMode; }

	void* CreateSegment(const char* name, size_t size, BOOL* newlyCreated);
	void* OpenSegment(const char* name, size_t size);
	void* CreateObjectSegment(int32_t typeId, const char* name, size_t objectSize, int32_t itemCount, BOOL* newlyCreated);
	BOOL CloseSegment(const char* name);
	BOOL DestroySegment(const char* name);

private:
	struct SEGMENT
	{
		SEGMENT();
		size_t size;
		void* addr;
#if defined(_WIN32) || defined(_WIN64)
		HANDLE handle;
#else
		int fd;
		std::string shmName;
#endif
	};

private:
	CAngelShmModule();
	~CAngelShmModule();
	CAngelShmModule(const CAngelShmModule&);
	CAngelShmModule& operator=(const CAngelShmModule&);

	std::string NormalizeName(const char* name) const;
	void ReleaseSegment(SEGMENT& segment);

private:
	ANGEL_OBJ_MODE m_InitMode;
	ANGEL_OBJ_MODE m_CreateMode;
	std::map<std::string, SEGMENT> m_Segments;
};

class CAngelShmPlugin : public CAngelPlugin
{
public:
	const char* GetPluginName() const { return "AngelShmPlugin"; }
	BOOL Awake();
	BOOL Shut();
	BOOL Finalize();
};

#endif // ANGEL_SHM_MODULE_H_
