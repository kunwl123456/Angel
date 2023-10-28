#ifndef _SHM_H__
#define _SHM_H__

#define MAGIC_USED_TAG       (0x87654321)
#define POOL_NAME_LEN        (128)
#define POOL_MAGIC_FREE_TAG  (0xABCDEF10)

void* shm_create(const char* pcszShmName, int32_t nSize);
void* mmap_ex(void* a_pvStart, size_t a_iLength, int a_iProt, int a_iFlags, HANDLE a_iFile, off_t a_iOffset);
void* shm_open(const char* pcszShmName, void* pAddr);

void* shm_open_readonly(const char* pcszShmName,void* pAddr);
BOOL  shm_find(const char* pcszShmName);

BOOL  shm_destroy(const char* pcszShmName);
void* shmv_create(int32_t nKey,uint64_t qwSize);

void* shmv_open(int32_t nKey, void* pAddr);

typedef BOOL(*CHECK_VMA_OVERLAY_FUNC_t)(uint64_t qwStartAddr, uint64_t qwEndAddr);
void set_check_vma_overlap_func(CHECK_VMA_OVERLAY_FUNC_t func);

CHECK_VMA_OVERLAY_FUNC_t get_check_vma_overlap_func(void);



#endif  //_SHM_H__
















