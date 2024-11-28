#include "fram_server/fram_server.h"

BOOL CFramServer::m_bIsDaemon = true;
BOOL CFramServer::m_bKickOnlineAcnt = false;
BOOL CFramServer::m_bIsExit = false;

CFramServer::CFramServer()
{

}

BOOL CFramServer::CanExit()
{
    return IsExited();
}

const char* CFramServer::GetEnvName()
{
    static char* cEnvName = getenv("ENV_NAME");
    if (!cEnvName || strlen(cEnvName) == 0)
    {
        return "test";
    }
    return cEnvName;
}