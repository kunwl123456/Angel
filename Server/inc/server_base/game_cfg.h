#ifndef _H_GAME_CFG_H__
#define _H_GAME_CFG_H__

#include "define/server_def.h"

class CGameCfg
{
public:
	CGameCfg();
	~CGameCfg();

	BOOL Load(const char* path);
	const char* GetConfigPath() const;

private:
	char m_ConfigPath[MAX_SVR_CONFIG_DIR];
};

#endif // _H_GAME_CFG_H__
