#include "stdafx.h"

#include "team_server.h"

CTeamServerApp::CTeamServerApp()
	:CSsGameServerApp(svrTS)
{
}

const char* CTeamServerApp::GetName() const
{
	return "team_server";
}

int main(int argc, char* argv[])
{
	static CTeamServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
