#include "stdafx.h"

#include "cross_server.h"

CCrossServerApp::CCrossServerApp()
	:CSsGameServerApp(svrCS)
{
}

const char* CCrossServerApp::GetName() const
{
	return "cross_server";
}

int main(int argc, char* argv[])
{
	static CCrossServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
