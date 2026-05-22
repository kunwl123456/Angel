#include "stdafx.h"

#include "base_lib/nf_shm/angel_shm_module.h"
#include "base_lib/nf_shm/angel_shm_registry.h"

#if !defined(_WIN32) && !defined(_WIN64)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

CAngelShmModule::SEGMENT::SEGMENT()
	: size(0),
	  addr(NULL)
#if defined(_WIN32) || defined(_WIN64)
	  , handle(NULL)
#else
	  , fd(-1)
#endif
{
}

CAngelShmModule& CAngelShmModule::Instance()
{
	static CAngelShmModule s_Module;
	return s_Module;
}

CAngelShmModule::CAngelShmModule()
	: m_InitMode(ANGEL_OBJ_MODE_INIT),
	  m_CreateMode(ANGEL_OBJ_MODE_INIT)
{
}

CAngelShmModule::~CAngelShmModule()
{
	Shut();
}

BOOL CAngelShmModule::Init()
{
	return TRUE;
}

BOOL CAngelShmModule::Shut()
{
	for (std::map<std::string, SEGMENT>::iterator it = m_Segments.begin(); it != m_Segments.end(); ++it)
	{
		ReleaseSegment(it->second);
	}
	m_Segments.clear();
	return TRUE;
}

std::string CAngelShmModule::NormalizeName(const char* name) const
{
	std::string value = name ? name : "angel_shm";
	for (size_t i = 0; i < value.size(); ++i)
	{
		char& ch = value[i];
		if (ch == '\\' || ch == '/' || ch == ':' || ch == ' ')
		{
			ch = '_';
		}
	}
#if !defined(_WIN32) && !defined(_WIN64)
	if (value.empty() || value[0] != '/')
	{
		value.insert(value.begin(), '/');
	}
#endif
	return value;
}

void* CAngelShmModule::CreateSegment(const char* name, size_t size, BOOL* newlyCreated)
{
	if (newlyCreated)
	{
		*newlyCreated = FALSE;
	}
	if (!name || size == 0)
	{
		return NULL;
	}

	const std::string normalizedName = NormalizeName(name);
	std::map<std::string, SEGMENT>::iterator found = m_Segments.find(normalizedName);
	if (found != m_Segments.end())
	{
		return found->second.addr;
	}

	SEGMENT segment;
	segment.size = size;

#if defined(_WIN32) || defined(_WIN64)
	segment.handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(size), normalizedName.c_str());
	if (!segment.handle)
	{
		return NULL;
	}
	if (GetLastError() != ERROR_ALREADY_EXISTS && newlyCreated)
	{
		*newlyCreated = TRUE;
	}
	segment.addr = MapViewOfFile(segment.handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (!segment.addr)
	{
		ReleaseSegment(segment);
		return NULL;
	}
#else
	segment.shmName = normalizedName;
	segment.fd = shm_open(normalizedName.c_str(), O_CREAT | O_RDWR, 0666);
	if (segment.fd < 0)
	{
		return NULL;
	}

	struct stat st;
	if (fstat(segment.fd, &st) == 0 && st.st_size == 0 && newlyCreated)
	{
		*newlyCreated = TRUE;
	}
	if (ftruncate(segment.fd, static_cast<off_t>(size)) != 0)
	{
		ReleaseSegment(segment);
		return NULL;
	}
	segment.addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, segment.fd, 0);
	if (segment.addr == MAP_FAILED)
	{
		segment.addr = NULL;
		ReleaseSegment(segment);
		return NULL;
	}
#endif

	m_Segments[normalizedName] = segment;
	return segment.addr;
}

void* CAngelShmModule::OpenSegment(const char* name, size_t size)
{
	if (!name || size == 0)
	{
		return NULL;
	}

	const std::string normalizedName = NormalizeName(name);
	std::map<std::string, SEGMENT>::iterator found = m_Segments.find(normalizedName);
	if (found != m_Segments.end())
	{
		return found->second.addr;
	}

	SEGMENT segment;
	segment.size = size;

#if defined(_WIN32) || defined(_WIN64)
	segment.handle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, normalizedName.c_str());
	if (!segment.handle)
	{
		return NULL;
	}
	segment.addr = MapViewOfFile(segment.handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (!segment.addr)
	{
		ReleaseSegment(segment);
		return NULL;
	}
#else
	segment.shmName = normalizedName;
	segment.fd = shm_open(normalizedName.c_str(), O_RDWR, 0666);
	if (segment.fd < 0)
	{
		return NULL;
	}
	segment.addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, segment.fd, 0);
	if (segment.addr == MAP_FAILED)
	{
		segment.addr = NULL;
		ReleaseSegment(segment);
		return NULL;
	}
#endif

	m_Segments[normalizedName] = segment;
	return segment.addr;
}

void* CAngelShmModule::CreateObjectSegment(int32_t typeId, const char* name, size_t objectSize, int32_t itemCount, BOOL* newlyCreated)
{
	if (typeId <= 0 || objectSize == 0 || itemCount < 0)
	{
		return NULL;
	}
	const size_t count = itemCount > 0 ? static_cast<size_t>(itemCount) : 1;
	const size_t totalSize = objectSize * count;
	return CreateSegment(name, totalSize, newlyCreated);
}

BOOL CAngelShmModule::CloseSegment(const char* name)
{
	const std::string normalizedName = NormalizeName(name);
	std::map<std::string, SEGMENT>::iterator found = m_Segments.find(normalizedName);
	if (found == m_Segments.end())
	{
		return TRUE;
	}

	ReleaseSegment(found->second);
	m_Segments.erase(found);
	return TRUE;
}

BOOL CAngelShmModule::DestroySegment(const char* name)
{
	const std::string normalizedName = NormalizeName(name);
	CloseSegment(name);
#if !defined(_WIN32) && !defined(_WIN64)
	shm_unlink(normalizedName.c_str());
#endif
	return TRUE;
}

void CAngelShmModule::ReleaseSegment(SEGMENT& segment)
{
	if (segment.addr)
	{
#if defined(_WIN32) || defined(_WIN64)
		UnmapViewOfFile(segment.addr);
#else
		munmap(segment.addr, segment.size);
#endif
		segment.addr = NULL;
	}

#if defined(_WIN32) || defined(_WIN64)
	if (segment.handle)
	{
		CloseHandle(segment.handle);
		segment.handle = NULL;
	}
#else
	if (segment.fd >= 0)
	{
		close(segment.fd);
		segment.fd = -1;
	}
#endif
}

BOOL CAngelShmPlugin::Awake()
{
	if (!CAngelShmRegistry::Instance().Validate())
	{
		return FALSE;
	}
	return CAngelShmModule::Instance().Init();
}

BOOL CAngelShmPlugin::Shut()
{
	return CAngelShmModule::Instance().Shut();
}

BOOL CAngelShmPlugin::Finalize()
{
	return TRUE;
}
