#include "stdafx.h"

#include "guild_server.h"

CGuildServerApp::CGuildServerApp()
	:CSsGameServerApp(svrGuild)
{
}

const char* CGuildServerApp::GetName() const
{
	return "guild_server";
}

int main(int argc, char* argv[])
{
	static CGuildServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
