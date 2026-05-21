#include "stdafx.h"

#include "gateway_server.h"

CGatewayServerApp::CGatewayServerApp()
	:CSsGameServerApp(svrGW)
{
}

const char* CGatewayServerApp::GetName() const
{
	return "gateway_server";
}

int main(int argc, char* argv[])
{
	static CGatewayServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}