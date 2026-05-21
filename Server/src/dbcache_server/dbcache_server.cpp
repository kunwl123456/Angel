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

int main(int argc, char* argv[])
{
	static CDbCacheServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}