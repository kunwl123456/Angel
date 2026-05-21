#include "stdafx.h"

#include "server_base/ss_game_server_app.h"

CSsGameServerApp::CSsGameServerApp(SERVER_TYPE serverType)
	:m_ServerType(serverType)
{
}

CSsGameServerApp::~CSsGameServerApp()
{
}

BOOL CSsGameServerApp::init(int argc, char* argv[])
{
	return CSsGameApp::init(argc, argv);
}

SERVER_TYPE CSsGameServerApp::GetServerType() const
{
	return m_ServerType;
}

BOOL CSsGameServerApp::OnInit(BOOL bResume)
{
	(void)bResume;
	printf("%s init, server_type=%d\n", GetName(), static_cast<int>(m_ServerType));
	return TRUE;
}

BOOL CSsGameServerApp::OnQuit()
{
	printf("%s quit\n", GetName());
	return TRUE;
}
