#ifndef _DBCACHE_SERVER_H__
#define _DBCACHE_SERVER_H__

#include "server_base/ss_game_server_app.h"
#include "game_data/role_data.h"

class CDbCacheServerApp : public CSsGameServerApp
{
public:
	CDbCacheServerApp();
	virtual const char* GetName() const override;

protected:
	virtual BOOL OnInit(BOOL bResume) override;
	virtual BOOL OnQuit() override;

private:
	CRoleDataCache m_RoleCache;
};

#endif //_DBCACHE_SERVER_H__