#include "stdafx.h"
#include "dbmanager_server.h"

CDbManagerServerApp::CDbManagerServerApp()
	:CSsGameServerApp(svrInvalid)
{
}

const char* CDbManagerServerApp::GetName() const
{
	return "dbmanager_server";
}

int main(int argc, char* argv[])
{
	static CDbManagerServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}