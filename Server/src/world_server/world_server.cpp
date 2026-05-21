#include "stdafx.h"

#include "world_server.h"

CWorldServerApp::CWorldServerApp()
	:CSsGameServerApp(svrWS)
{
}

const char* CWorldServerApp::GetName() const
{
	return "world_server";
}

int main(int argc, char* argv[])
{
	static CWorldServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
