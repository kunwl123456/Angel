#ifndef _TEAM_SERVER_H__
#define _TEAM_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CTeamServerApp : public CSsGameServerApp
{
public:
	CTeamServerApp();
	virtual const char* GetName() const override;
};

#endif // _TEAM_SERVER_H__
