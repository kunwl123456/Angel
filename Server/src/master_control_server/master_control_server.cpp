#include "stdafx.h"

#include "master_control_server.h"

CMasterControlServerApp::CMasterControlServerApp()
	:CSsGameServerApp(svrMCS)
{
}

const char* CMasterControlServerApp::GetName() const
{
	return "master_control_server";
}

int main(int argc, char* argv[])
{
	static CMasterControlServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
