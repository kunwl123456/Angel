#include "stdafx.h"
#include "base_lib/vm_space.h"
#include "base_lib/nf_shm/angel_shm_module.h"
#include "base_lib/hash.h"
#include "base_lib/rb_tree.h"

#ifdef __linux__
#include <link.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#endif

static BOOL g_LastSuccess = TRUE;

BOOL vm_create_space(int32_t nKey, uint64_t qwSize, BOOL bTryHugePage)
{
	(void)nKey;
	(void)qwSize;
	(void)bTryHugePage;
	g_LastSuccess = CAngelShmModule::Instance().Init();
	return g_LastSuccess;
}

BOOL vm_open_space(int32_t nKey)
{
	(void)nKey;
	g_LastSuccess = CAngelShmModule::Instance().Init();
	return g_LastSuccess;
}

void* vm_new_chunk(const char* szName, uint64_t qwChunkSize)
{
	BOOL newlyCreated = FALSE;
	void* pData = CAngelShmModule::Instance().CreateSegment(szName, static_cast<size_t>(qwChunkSize), &newlyCreated);
	g_LastSuccess = pData != NULL;
	return pData;
}

void* vm_find_chunk(const char* szName)
{
	void* pData = CAngelShmModule::Instance().OpenSegment(szName, 1);
	g_LastSuccess = pData != NULL;
	return pData;
}

BOOL vm_destroy_space(void)
{
	g_LastSuccess = CAngelShmModule::Instance().Shut();
	return g_LastSuccess;
}

BOOL vm_check_last_success(void)
{
	return g_LastSuccess;
}

BOOL vm_reset_last_success(void)
{
	g_LastSuccess = FALSE;
	return TRUE;
}

BOOL vm_set_last_success(void)
{
	g_LastSuccess = TRUE;
	return TRUE;
}

BOOL vm_mem_usage(struct VMUsage* pUsageData)
{
	if (!pUsageData)
	{
		return FALSE;
	}
	pUsageData->llChunkCnt = 0;
	pUsageData->llUsedSize = 0;
	pUsageData->llTotalSize = 0;
	return TRUE;
}

