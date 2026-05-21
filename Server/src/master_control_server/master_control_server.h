#ifndef _MASTER_CONTROL_SERVER_H__
#define _MASTER_CONTROL_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CMasterControlServerApp : public CSsGameServerApp
{
public:
	CMasterControlServerApp();
	virtual const char* GetName() const override;
};

#endif // _MASTER_CONTROL_SERVER_H__
