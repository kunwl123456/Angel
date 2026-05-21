#include "stdafx.h"
#include "scene_server.h"

CSceneServerApp::CSceneServerApp()
	:CSsGameServerApp(svrSS)
{
}

const char* CSceneServerApp::GetName() const
{
	return "scene_server";
}

BOOL CSceneServerApp::OnInit(BOOL bResume)
{
	CSsGameServerApp::OnInit(bResume);
	// SceneServer consumes MovementFragment-style state sync data in future AOI logic.
	return TRUE;
}

int main(int argc, char* argv[])
{
	static CSceneServerApp app;
	if (!app.init(argc, argv))
	{
		return -1;
	}
	return app.Run();
}