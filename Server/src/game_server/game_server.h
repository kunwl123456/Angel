#ifndef _GAME_SERVER_H__
#define _GAME_SERVER_H__

#include "fram_server/fram_server.h"

class CGameServer :public CFramServer
{
public:
    CGameServer();
    virtual ~CGameServer();

public:

    BOOL Init(const char* szConfig);

    BOOL MainLoop(const char* szConfig);

    void CreateReloadThread(const char* szConfig);

    virtual BOOL canExit() override;

};

#endif //_GAME_SERVER_H__