#ifndef _H_SS_GAME_SERVER_APP_H__
#define _H_SS_GAME_SERVER_APP_H__

#include "define/server_def.h"
#include "server_base/ss_game_app.h"

class CSsGameServerApp : public CSsGameApp
{
public:
	explicit CSsGameServerApp(SERVER_TYPE serverType);
	virtual ~CSsGameServerApp();

	BOOL init(int argc, char* argv[]);
	SERVER_TYPE GetServerType() const;

protected:
	virtual BOOL OnInit(BOOL bResume) override;
	virtual BOOL OnQuit() override;

private:
	SERVER_TYPE m_ServerType;
};

#endif // _H_SS_GAME_SERVER_APP_H__
