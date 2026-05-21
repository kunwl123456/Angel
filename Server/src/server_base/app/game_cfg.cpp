#include "stdafx.h"
#include "server_base/game_cfg.h"

CGameCfg::CGameCfg()
{
	memset(m_ConfigPath, 0, sizeof(m_ConfigPath));
}

CGameCfg::~CGameCfg()
{
}

BOOL CGameCfg::Load(const char* path)
{
	memset(m_ConfigPath, 0, sizeof(m_ConfigPath));
	if (path && path[0] != '\0')
	{
#if defined(_WIN32) || defined(_WIN64)
		strncpy_s(m_ConfigPath, sizeof(m_ConfigPath), path, _TRUNCATE);
#else
		strncpy(m_ConfigPath, path, sizeof(m_ConfigPath) - 1);
#endif
	}
	return TRUE;
}

const char* CGameCfg::GetConfigPath() const
{
	return m_ConfigPath;
}