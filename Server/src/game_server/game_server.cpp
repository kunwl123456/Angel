#include "stdafx.h"
#include "game_server.h"
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
    return TRUE;
}


BOOL CGameServer::MainLoop(const char* szConfig)
{
    return TRUE;
}

void CGameServer::CreateReloadThread(const char* szConfig)
{

}

BOOL CGameServer::CanExit()
{
    return TRUE;
}

BOOL CGameServer::_InitProcConfig(const char* szConfig)
{
    return TRUE;
}


int32_t main(int argc, char* argv[])
{

    //1、init log cfg

    //2、
    char        sConfFilePath[SVR_PATH_MAX] = { 0 };
    if (!CGameServer::getInstance()->Init(sConfFilePath))
    {

        return -1;
    }

    CGameServer::getInstance()->MainLoop(sConfFilePath);

    printf("game_server ready122223333!\n");
    printf("game_server ready!\n");
    return 0;
}