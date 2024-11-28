#include "stdafx.h"
#include "process_error.h"
#include "queue/ringbuffer.h"
#include "vm_space.h"

void CRingBuff::init(uint32_t size)
{
	tail_index = 0;
	head_index = 0;
	vec_buf.size = size;
	assert_retnone(is_2power(vec_buf.size));
	vec_buf.buffer = (char*)((char*)this + sizeof(CRingBuff));

	//vec_buf.buffer = (char*)vm_find_chunk(name);
	//vec_buf.buffer = vec_buf.buffer == NULL ? (char*)vm_find_chunk(name, size): vec_buf.buffer;

	assert_retnone(vec_buf.buffer);
}

void CRingBuff::write(char data)
{
	if (is_full())
		tail_index = ((tail_index + 1)&(vec_buf.size - 1));
	vec_buf.buffer[head_index] = data;

	//在这个范围内往前移动一位
	head_index = ((head_index + 1)&(vec_buf.size - 1));
}

void CRingBuff::write_arr(const char* data, uint32_t size)
{
	for (auto i = 0; i < size; ++i)
		write(data[i]);
}


uint32_t CRingBuff::read(char* data)
{
	*data = vec_buf.buffer[tail_index];
	tail_index = ((tail_index + 1)&(vec_buf.size - 1));
	return 1;
}

uint32_t CRingBuff::read_arr(char* data, uint32_t len)
{
	if (is_empty())
		return 0;
	char* data_ptr = data;
	uint32_t cnt = 0;
	while ((cnt < len) && read(data_ptr))
	{
		cnt++;
		data_ptr++;
	}
	return cnt;

}

uint32_t CRingBuff::peek(char* data, uint32_t index)
{
	if (index >= num_items())
		return 0;

	uint32_t data_index = ((tail_index + index)&(vec_buf.size - 1));
	*data = vec_buf.buffer[data_index];
	return 1;

}

BOOL CRingBuff::is_2power(size_t size)
{
	return (size!=0)&&(size & (size - 1) == 0);
}


BOOL CRingBuff::peek_arr(void* data, int len)
{
	if (remain() < len)
		return false;
	uint32_t cnt = 0;
	char* data_ptr = (char*)data;
	while ((cnt < len) && peek(data_ptr++, cnt++));

	return true;
}

CRingBuff* create_ringbuff(char* name, uint64_t qwChunkSize)
{
	void* pRet = vm_find_chunk(name);
	pRet = (pRet == NULL) ? new(vm_new_chunk(name, qwChunkSize))CRingBuff : pRet;
	return (CRingBuff*)pRet;
}


















