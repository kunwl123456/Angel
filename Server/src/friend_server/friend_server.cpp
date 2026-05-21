#include "stdafx.h"

#include "friend_server.h"

CFriendServerApp::CFriendServerApp()
	:CSsGameServerApp(svrFS)
{
}

const char* CFriendServerApp::GetName() const
{
	return "friend_server";
}

int main(int argc, char* argv[])
{
	static CFriendServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
