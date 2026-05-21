#ifndef _GLOBAL_SERVER_H__
#define _GLOBAL_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CGlobalServerApp : public CSsGameServerApp
{
public:
	CGlobalServerApp();
	virtual const char* GetName() const override;
};

#endif // _GLOBAL_SERVER_H__
