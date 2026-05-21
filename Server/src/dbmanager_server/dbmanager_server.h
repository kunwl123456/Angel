#ifndef _DBMANAGER_SERVER_H__
#define _DBMANAGER_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CDbManagerServerApp : public CSsGameServerApp
{
public:
	CDbManagerServerApp();
	virtual const char* GetName() const override;
};

#endif //_DBMANAGER_SERVER_H__