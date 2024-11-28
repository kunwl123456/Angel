#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#pragma pack(push,1)

class CRingBuff
{
public:
	void init(uint32_t size);
	void write(char data);
	void write_arr(const char* data, uint32_t size);
	uint32_t read(char* data);
	uint32_t read_arr(char* data, uint32_t len);
	
	//读取尾指针偏移index个位置后该数据（尾指针位置不变）
	uint32_t peek(char* data, uint32_t index);
	BOOL peek_arr(void* data, int len);

public:
	inline BOOL is_empty()
	{
		return (head_index == tail_index);
	}


	inline uint32_t is_full()
	{
		return ((head_index - tail_index)& (vec_buf.size - 1)) == (vec_buf.size - 1);
	}

	//! have items(表示有内容吗)
	inline uint32_t num_items() {
		return ((head_index - tail_index)&(vec_buf.size - 1));
	}

	//!remain len
	inline uint32_t remain()
	{
		return vec_buf.size - num_items() - 1;
	}

private:
	BOOL is_2power(size_t size);
private:
	uint32_t tail_index;
	uint32_t head_index;
	struct  
	{
		uint32_t size;
		char* buffer;
	}vec_buf;

};

#pragma pack(pop)

CRingBuff* create_ringbuff(char* name, uint64_t qwChunkSize);

#endif //RINGBUFFER_H






