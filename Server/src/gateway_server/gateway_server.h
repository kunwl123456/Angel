#ifndef _GATEWAY_SERVER_H__
#define _GATEWAY_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CGatewayServerApp : public CSsGameServerApp
{
public:
	CGatewayServerApp();
	virtual const char* GetName() const override;
};

#endif //_GATEWAY_SERVER_H__