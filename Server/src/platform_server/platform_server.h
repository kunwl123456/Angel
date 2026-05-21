#ifndef _PLATFORM_SERVER_H__
#define _PLATFORM_SERVER_H__

#include "server_base/ss_game_server_app.h"

class CPlatformServerApp : public CSsGameServerApp
{
public:
	CPlatformServerApp();
	virtual const char* GetName() const override;
};

#endif // _PLATFORM_SERVER_H__
