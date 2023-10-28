#include <stdafx.h>
#include "shm.h"
#include "base_lib/process_error.h"

CHECK_VMA_OVERLAY_FUNC_t __CheckVmaOverlapFunc = NULL;

void* shm_create(const char* pcszShmName, int32_t nSize)
{
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	HANDLE  pHandle = NULL;
	void*   pAddr = NULL;

#if defined(WIN32) || defined(WIN64)
	pHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nSize, pcszShmName);
	if (!pHandle) goto Exit0;

	pAddr = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!pAddr) goto Exit0;
#else

	//nHandle = shm_open(pcszShmName, O_RDWR | O_CREATE, S_IRUSR | S_IWUSR);
	//if (nHandle == -1) goto Exit0;

	nHandle = open(pcszShmName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (nHandle <= 0) goto Exit0;

	nRetCode = ftruncate(nHandle, nSize);
	if (nRetCode != 0) goto Exit0;

	pAddr = mmap(NULL, nSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	if (pAddr == (void*)-1 && pAddr != NULL) goto Exit0;

#endif

	return pAddr;
Exit0:
#if defined(WIN32) || defined(WIN64)
	if (pHandle)
		CloseHandle(pHandle);
#else
	if (nHandle > 0)
		close(nHandle);
#endif

	return NULL;
}

void* shm_open(const char* pcszShmName, void* pAddr)
{
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	HANDLE  pHandle = NULL;
	int32_t nSize = 0;
	struct  stat st;

#if defined(WIN32) || defined(WIN64)
	pHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, pcszShmName);
	if (!pHandle) goto Exit0;

	pAddr = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!pAddr) goto Exit0;
#else
	nHandle = open(pcszShmName, O_RDWD, S_IRUSR | S_IWUSR);
	if (nHandle <= 0) goto Exit0;

	nRetCode = fstat(nHandle, &st);
	if (nRetCode != 0) goto Exit0;
	nSize = st.st_size;

	pAddr = mmap(pAddr, nSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	if (pAddr == (void*)-1 || pAddr == NULL) goto Exit0;

#endif

	return pAddr;
Exit0:
#if defined(WIN32) || defined(WIN64)
	if (pHandle)
		CloseHandle(pHandle);
#else
	if (nHandle > 0)
		close(nHandle);
#endif
	return NULL;
}

void* shm_open_readonly(const char* pcszShmName, void* pAddr)
{
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	HANDLE  pHandle = NULL;
	int32_t nSize = 0;
	struct  stat st;

#if defined(WIN32) || defined(WIN64)
	pHandle = OpenFileMapping(FILE_MAP_READ, 0, pcszShmName);
	if (!pHandle) goto Exit0;

	pAddr = MapViewOfFile(pHandle, FILE_MAP_READ, 0, 0, 0);
	if (!pAddr) goto Exit0;

#else
	nHandle = open(pcszShmName, O_RDONLY, S_IRUSR | S_IWUSR);
	if (nHandle <= 0) goto Exit0;

	nRetCode = fstat(nHandle, &st);
	if (nRetCode != 0) goto Exit0;
	nSize = st.st_size;

	pAddr = mmap(pAddr, nSize, PROT_READ, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	if (pAddr == (void*)-1 || pAddr == NULL);

#endif

	return pAddr;

Exit0:
#if defined(WIN32) || defined(WIN64)
	if (pHandle)
		CloseHandle(pHandle);
#else
	if (nHandle > 0)
		close(nHandle);
#endif
	return NULL;
}

BOOL  shm_find(const char* pcszShmName)
{
	int32_t nHandle = -1;
	int32_t nRetCode = 0;
	HANDLE  pHandle = NULL;

#if defined(WIN32) || defined(WIN64)
	pHandle = OpenFileMapping(FILE_MAP_READ, 0, pcszShmName);
	if (!pHandle) goto Exit0;

	CloseHandle(pHandle);
#else
	nHandle = shm_open(pcszShmName, O_RDONLY, 0);
	if (pHandle == -1) goto Exit0;

	close(nHandle);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

BOOL  shm_destroy(const char* pcszShmName)
{
	int32_t nRetCode = 0;
	int32_t nHandle = -1;
	HANDLE  pHandle = NULL;
	void*   pAddr = NULL;
	struct stat st;

#if defined(WIN32) || defined(WIN64)
	pHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, pcszShmName);
	if (!pHandle) goto Exit0;

	pAddr = MapViewOfFile(pHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = UnmapViewOfFile(pAddr);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CloseHandle(pHandle);
	LOG_PROCESS_ERROR(nRetCode);

#else
	nHandle = open(pcszShmName, O_RDWR , S_IRUSR | S_IWUSR);
	LOG_PROCESS_ERROR(nHandle != -1);

	nRetCode = fstat(nHandle, &st);
	LOG_PROCESS_ERROR(nRetCode == 0);
	nSize = st.st_size;

	pAddr = mmap(NULL, nSize, PROT_READ | PROT_WRITE
		, MAP_SHARED | MAP_POPULATE, nHandle, 0);
	LOG_PROCESS_ERROR(pAddr);

	nRetCode = munmap(pAddr, nSize);
	LOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = shm_unlink(psczShmName);
	LOG_PROCESS_ERROR(nRetCode == 0);

	close(nHandle);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

typedef BOOL(*CHECK_VMA_OVERLAY_FUNC_t)(uint64_t qwStartAddr, uint64_t qwEndAddr);

void set_check_vma_overlap_func(CHECK_VMA_OVERLAY_FUNC_t func)
{
	__CheckVmaOverlapFunc = func;

	return;
}

CHECK_VMA_OVERLAY_FUNC_t get_check_vma_overlap_func(void)
{
	return __CheckVmaOverlapFunc;
}










