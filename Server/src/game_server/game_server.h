#ifndef _GAME_SERVER_H__
#define _GAME_SERVER_H__

#include "fram_server/fram_server.h"
#include "server_base/ss_game_server_app.h"
#include    <mutex>

class CGameServer :public CFramServer
{
public:
    CGameServer();
    virtual ~CGameServer();

public:
    static CGameServer* getInstance();
    static void releaseInstance();
    static std::mutex mutex;

    BOOL Init(const char* szConfig);

    BOOL MainLoop(const char* szConfig);

    void CreateReloadThread(const char* szConfig);
    BOOL _BeginPreExit();

    virtual BOOL CanExit() override;
private:
    CGameServer(const CGameServer&) = delete;
    CGameServer& operator=(const CGameServer&) = delete;
private:
    BOOL _InitProcConfig(const char* szConfig);
    static CGameServer* instance;
};

class CGameServerApp : public CSsGameServerApp
{
public:
	CGameServerApp();
	virtual ~CGameServerApp();
	virtual const char* GetName() const override;

protected:
	virtual BOOL OnInit(BOOL bResume) override;
	virtual BOOL OnTick() override;
	virtual BOOL OnQuit() override;
	virtual BOOL OnFini() override;

private:
	BOOL m_BeginExit;
};

#endif //_GAME_SERVER_H__