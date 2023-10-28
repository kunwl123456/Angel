#include "stdafx.h"

#include <algorithm>

#include "define/server_def.h"

#include "server_base/server_base.h"
#include "server_base/ss_game_app.h"
#include "server_base/game_log.h"
#include "server_base/game_cfg.h"
#include "server_base/game_ver.h"


int32_t g_nAreaID;

CSsGameApp* CSsGameApp::gApp = 0;

void _WinSignalHandler(int signal)
{
	CSsGameApp::get_app().SetStopFlag(TRUE);
}

CSsGameApp& CSsGameApp::get_app()
{
	return *gApp;
}




















