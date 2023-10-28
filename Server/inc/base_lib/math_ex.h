#ifndef _MATH_EX_H_
#define _MATH_EX_H_





void srand_ex(uint32_t dwSeed);
int64_t rand_ex(int64_t llRange);
int64_t rand_ex(int64_t llMin, int64_t llMax);
uint32_t get_srand_seed_ex();

int32_t rand_from_arr64(const int64_t* nRange, int32_t nRangeCount);
int32_t range_from_arr(const int32_t* nRange, int32_t nRangeCount);
int32_t rand_from_arr_direct(const int32_t* nRange,int32_t nRangeCount);

BOOL init_math(void);
void uninit_math(void);




#endif //_MATH_EX_H_