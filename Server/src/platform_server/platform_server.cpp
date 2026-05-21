#include "stdafx.h"

#include "platform_server.h"

CPlatformServerApp::CPlatformServerApp()
	:CSsGameServerApp(svrPS)
{
}

const char* CPlatformServerApp::GetName() const
{
	return "platform_server";
}

int main(int argc, char* argv[])
{
	static CPlatformServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
