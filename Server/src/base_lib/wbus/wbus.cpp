#include "stdafx.h"
#include <app/tapp.h>
#include <wbus_def.h>
#include "wbus.h"
#include "base_lib/queue/ringbuffer.h"
#include <wbus_network.h>
#include <define/server_def.h>
//#include <zlib.h>

#include "base_lib/error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

	int wbus_addr(IN int area, IN int zone, IN int s_type, IN int index)
	{
		return (WBUSADDR)MAKE_SERVER_ADDR(area, zone, s_type, index);
	}

	int wbus_addr_aton(IN const char* a_pszAddr, OUT WBUSADDR* a_piAddr)
	{
		if (!a_pszAddr || !a_piAddr) return AGERR_NULL_POINTER;

		int area, zone, stype, index;
		sscanf(a_pszAddr, "%d.%d.%d.%d", &area, &zone, &stype, &index);

		*a_piAddr = (WBUSADDR)MAKE_SERVER_ADDR(area, zone, stype, index);

		return AGERR_SUCCESS;
	}

	char* wbus_addr_ntoa(IN WBUSADDR a_iAddr)
	{
		static char result[16] = { 0 };

		int area = GET_SERVER_AREA(a_iAddr);
		int zone = GET_SERVER_ZONE(a_iAddr);
		int stype = GET_SERVER_TYPE(a_iAddr);
		int index = GET_SERVER_INDEX(a_iAddr);
		sprintf(result, "%d.%d.%d.%d", area, zone, stype, index);

		return result;
	}

	int wbus_addr_ntoa_with_buf(IN WBUSADDR a_iAddr, char* buf, int len)
	{


		return AGERR_SUCCESS;
	}


#ifdef __cplusplus
}
#endif









