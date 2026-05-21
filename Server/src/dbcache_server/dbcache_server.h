#ifndef _DBCACHE_SERVER_H__
#define _DBCACHE_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CDbCacheServerApp : public CSsGameServerApp
{
public:
	CDbCacheServerApp();
	virtual const char* GetName() const override;
};

#endif //_DBCACHE_SERVER_H__