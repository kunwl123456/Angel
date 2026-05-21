#include "stdafx.h"

#include "global_server.h"

CGlobalServerApp::CGlobalServerApp()
	:CSsGameServerApp(svrGlobal)
{
}

const char* CGlobalServerApp::GetName() const
{
	return "global_server";
}

int main(int argc, char* argv[])
{
	static CGlobalServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}
