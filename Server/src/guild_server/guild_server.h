#ifndef _GUILD_SERVER_H__
#define _GUILD_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CGuildServerApp : public CSsGameServerApp
{
public:
	CGuildServerApp();
	virtual const char* GetName() const override;
};

#endif // _GUILD_SERVER_H__
