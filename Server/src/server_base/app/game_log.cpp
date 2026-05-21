#include "stdafx.h"
#include "server_base/game_log.h"

#include <stdarg.h>

CGameLog::CGameLog()
{
}

CGameLog::~CGameLog()
{
}

void CGameLog::Info(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("[INFO] ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}

void CGameLog::Error(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("[ERROR] ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}













