#include "stdafx.h"
#include "dbcache_server.h"

CDbCacheServerApp::CDbCacheServerApp()
	:CSsGameServerApp(svrInvalid)
{
}

const char* CDbCacheServerApp::GetName() const
{
	return "dbcache_server";
}

BOOL CDbCacheServerApp::OnInit(BOOL bResume)
{
	(void)bResume;
	if (!CSsGameServerApp::OnInit(bResume))
	{
		return FALSE;
	}
	return m_RoleCache.Init();
}

BOOL CDbCacheServerApp::OnQuit()
{
	m_RoleCache.FlushDirtyRoles();
	return CSsGameServerApp::OnQuit();
}

int main(int argc, char* argv[])
{
	static CDbCacheServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}