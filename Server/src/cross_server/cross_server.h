#ifndef _CROSS_SERVER_H__
#define _CROSS_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CCrossServerApp : public CSsGameServerApp
{
public:
	CCrossServerApp();
	virtual const char* GetName() const override;
};

#endif // _CROSS_SERVER_H__
