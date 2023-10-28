#ifndef _WBUS_H__
#define _WBUS_H__

#include "wbus_def.h"
#include "define/server_def.h"

#ifdef __cplusplus
extern "C" {
#endif
	int wbus_addr(IN int area, IN int zone, IN int s_type, IN int index);
	int wbus_addr_aton(IN const char* a_pszAddr, OUT WBUSADDR* a_piAddr);
	char* wbus_addr_ntoa(IN WBUSADDR a_iAddr);
	int wbus_addr_ntoa_with_buf(IN WBUSADDR a_iAddr, char* buf, int len);

#ifdef __cplusplus
}
#endif


#endif //_WBUS_H__

















