
#ifndef FRAM_SERVER_H
#define FRAM_SERVER_H


class CFramServer
{
public:
    CFramServer();
    virtual ~CFramServer() {}

    static void SetDaemon(BOOL bIsDemon) { m_bIsDaemon = bIsDemon; }
    static BOOL IsDaemon() { return m_bIsDaemon; }

    static void SetExited() { m_bIsExit = true; }
    static BOOL IsExited() { return m_bIsExit; }



    //------------------------
    virtual BOOL CanExit();
    const char* GetEnvName();
private:
    //start ready to exit
    virtual BOOL _BeginPreExit() = 0;

    BOOL _InitSvrVersion();

private:
    static BOOL m_bIsExit;
    static BOOL m_bIsDaemon;
    static BOOL m_bKickOnlineAcnt;

};



#endif






















