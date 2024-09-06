#ifndef _GAME_SERVER_H__
#define _GAME_SERVER_H__

#include "fram_server/fram_server.h"
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

#endif //_GAME_SERVER_H__