#ifndef _WORLD_SERVER_H__
#define _WORLD_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CWorldServerApp : public CSsGameServerApp
{
public:
	CWorldServerApp();
	virtual const char* GetName() const override;
};

#endif // _WORLD_SERVER_H__
