#include "stdafx.h"
#include "game_server.h"
#include "game_world/game_world.h"
#include <iostream>


CGameServer* CGameServer::instance = nullptr;
std::mutex CGameServer::mutex;

CGameServer::CGameServer()
{

}


CGameServer::~CGameServer()
{

}
CGameServer* CGameServer::getInstance()
{
    if (!instance)
    {
        std::lock_guard<std::mutex> lock(mutex); // 线程安全
        if (instance == nullptr) {
            instance = new CGameServer();
        }
    }
    return instance;
}

void CGameServer::releaseInstance()
{
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

BOOL CGameServer::_BeginPreExit()
{
    return TRUE;
}

BOOL CGameServer::Init(const char* szConfig)
{
    if (!_InitProcConfig(szConfig))
    {
        return FALSE;
    }
    if (!CGameWorld::instance().init())
    {
        return FALSE;
    }
    return TRUE;
}


BOOL CGameServer::MainLoop(const char* szConfig)
{
    (void)szConfig;
    CGameWorld::instance().mainloop();
    return TRUE;
}

void CGameServer::CreateReloadThread(const char* szConfig)
{

}

BOOL CGameServer::CanExit()
{
    return CFramServer::CanExit();
}

BOOL CGameServer::_InitProcConfig(const char* szConfig)
{
    (void)szConfig;
    return TRUE;
}


CGameServerApp::CGameServerApp()
    :CSsGameServerApp(svrGS),
    m_BeginExit(FALSE)
{
}

CGameServerApp::~CGameServerApp()
{
}

const char* CGameServerApp::GetName() const
{
    return "game_server";
}

BOOL CGameServerApp::OnInit(BOOL bResume)
{
    if (!CSsGameServerApp::OnInit(bResume))
    {
        return FALSE;
    }
    if (!CGameServer::getInstance()->Init(get_ctx().pszConfFile ? get_ctx().pszConfFile : ""))
    {
        return FALSE;
    }
    SetState(gsstReady);
    return TRUE;
}

BOOL CGameServerApp::OnTick()
{
    CGameServer::getInstance()->MainLoop(get_ctx().pszConfFile ? get_ctx().pszConfFile : "");
    return TRUE;
}

BOOL CGameServerApp::OnQuit()
{
    CGameServer* server = CGameServer::getInstance();
    if (!m_BeginExit)
    {
        server->_BeginPreExit();
        SetState(gsstStopWaitForC2gMsgClear);
        m_BeginExit = TRUE;
    }

    switch (GetState())
    {
    case gsstStopWaitForC2gMsgClear:
        SetState(gsstStopWaitForServerReply);
        break;
    case gsstStopWaitForServerReply:
        SetState(gsstStopWaitForSaveRole);
        break;
    case gsstStopWaitForSaveRole:
        SetState(gsstStopWaitForPoolClear);
        break;
    case gsstStopWaitForPoolClear:
        SetState(gsstStopComplete);
        break;
    case gsstStopComplete:
        SetState(gsstClosed);
        CFramServer::SetExited();
        break;
    default:
        SetState(gsstClosed);
        CFramServer::SetExited();
        break;
    }
    return server->CanExit();
}

BOOL CGameServerApp::OnFini()
{
    CGameWorld::instance().uninit();
    CGameServer::releaseInstance();
    return TRUE;
}

int32_t main(int argc, char* argv[])
{
    static CGameServerApp app;
    if (!app.init(argc, argv))
    {
        return -1;
    }
    return app.Run();
}