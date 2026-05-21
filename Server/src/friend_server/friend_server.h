#ifndef _FRIEND_SERVER_H__
#define _FRIEND_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CFriendServerApp : public CSsGameServerApp
{
public:
	CFriendServerApp();
	virtual const char* GetName() const override;
};

#endif // _FRIEND_SERVER_H__
