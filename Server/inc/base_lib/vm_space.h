#ifndef __vmspace_h__
#define __vmspace_h__

BOOL vm_create_space(int32_t nKey, uint64_t qwSize, BOOL bTryHugePage);
BOOL vm_open_space(int32_t nKey);

void* vm_new_chunk(const char* szName, uint64_t qwChunkSize);
void* vm_find_chunk(const char* szName);

BOOL vm_destroy_space(void);

BOOL vm_check_last_success(void);
BOOL vm_reset_last_success(void);
BOOL vm_set_last_success(void);

struct VMUsage
{
	uint64_t llChunkCnt;
	uint64_t llUsedSize;
	uint64_t llTotalSize;
};

BOOL vm_mem_usage(struct VMUsage* pUsageData);

#endif //__vmspace_h__



















