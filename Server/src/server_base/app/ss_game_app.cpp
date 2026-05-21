#include "stdafx.h"

#include <algorithm>

#include "define/server_def.h"

#include "server_base/server_base.h"
#include "server_base/ss_game_app.h"
#include "server_base/game_log.h"
#include "server_base/game_cfg.h"
#include "server_base/game_ver.h"

#include "app/tapp_runtime.h"
#include "base_lib/error_code.h"
#include "base_lib/wbus_network.h"

int32_t g_nAreaID;

CSsGameApp* CSsGameApp::gApp = 0;

void _WinSignalHandler(int signal)
{
	(void)signal;
	CSsGameApp::get_app().SetStopFlag(TRUE);
}

CSsGameApp& CSsGameApp::get_app()
{
	return *gApp;
}

void CSsGameApp::on_server_end_service(int32_t nServiceType, int32_t nServerAddr, int32_t nParam1, int32_t nParam2)
{
	(void)nServiceType;
	(void)nServerAddr;
	(void)nParam1;
	(void)nParam2;
}

CSsGameApp::CSsGameApp()
	:m_Quit(FALSE),
	m_Log(NULL),
	m_Cfg(NULL),
	m_NetworkStarted(FALSE),
	m_TimeZone(0),
	m_RealTime(0),
	m_RealTimestamp(0),
	m_StartTick(0),
	m_RealTick(0),
	m_CurFrame(0),
	m_State(0),
	m_StopTimer(0),
	m_llTimeDiff(0),
	m_bStopFlag(FALSE),
	m_llTimeDatModifyTime(0)
{
	memset(&m_Ctx, 0, sizeof(m_Ctx));
	memset(&m_CurLocalTime, 0, sizeof(m_CurLocalTime));
	gApp = this;
}

CSsGameApp::~CSsGameApp()
{
	if (gApp == this)
	{
		gApp = NULL;
	}
}

BOOL CSsGameApp::init(int argc, char* argv[])
{
	if (tapp_def_init(&m_Ctx, argc, argv, GetName(), this) != 0)
	{
		return FALSE;
	}

	m_Ctx.uiVersion = TAPP_MAKE_VERSION(MAJOR_VER, MINOR_VER, REV_VER, BUILD_VER);
	m_Ctx.pfnInit = __OnInit;
	m_Ctx.pfnQuit = __OnQuit;
	m_Ctx.pfnFini = __OnFini;
	m_Ctx.pfnProc = __OnProc;
	m_Ctx.pfnTick = __OnTick;
	m_Ctx.pfnReload = __OnReload;
	m_Ctx.pfnConnect = __OnConnect;
	m_Ctx.pfnDisconnect = __OnDisconnect;

	signal(SIGINT, _WinSignalHandler);
	signal(SIGTERM, _WinSignalHandler);
	return TRUE;
}

void CSsGameApp::Destroy()
{
	_OnDestroy();
	_OnFini();
}

int CSsGameApp::Run()
{
	return tapp_mainloop(&m_Ctx);
}

void CSsGameApp::GetWin(int& x, int& y)
{
	x = 0;
	y = 0;
}

CRedis* CSsGameApp::GetDb()
{
	return NULL;
}

int64_t CSsGameApp::read_diff_time()
{
	return m_llTimeDiff;
}

void CSsGameApp::write_diff_time(int64_t& llTimeDiff)
{
	m_llTimeDiff = llTimeDiff;
}

CGameLog& CSsGameApp::GetLog()
{
	static CGameLog s_Log;
	return m_Log ? *m_Log : s_Log;
}

CGameCfg& CSsGameApp::GetCfg()
{
	static CGameCfg s_Cfg;
	return m_Cfg ? *m_Cfg : s_Cfg;
}

CSsGameApp::STAT::BUS_STAT* CSsGameApp::_bus_stat(int32_t nPeerAddr)
{
	return &m_Stat.Buss[nPeerAddr];
}

void CSsGameApp::_cal_bus_stat()
{
}

void CSsGameApp::_print_bus_stat()
{
}

void CSsGameApp::_proc_console()
{
}

void CSsGameApp::_print_db_stat()
{
}

BOOL CSsGameApp::OnIniting(BOOL bResume)
{
	(void)bResume;
	return TRUE;
}

BOOL CSsGameApp::OnInit(BOOL bResume)
{
	(void)bResume;
	return TRUE;
}

BOOL CSsGameApp::OnInitFail()
{
	return TRUE;
}

BOOL CSsGameApp::OnDestroy()
{
	return TRUE;
}

BOOL CSsGameApp::OnDestroyed()
{
	return TRUE;
}

BOOL CSsGameApp::OnQuit()
{
	return TRUE;
}

BOOL CSsGameApp::OnFini()
{
	return TRUE;
}

void CSsGameApp::OnProc()
{
}

BOOL CSsGameApp::OnTick()
{
	return TRUE;
}

void CSsGameApp::OnFrame(int64_t frame)
{
	(void)frame;
}

void CSsGameApp::OnTimer1()
{
}

void CSsGameApp::OnTimer10()
{
}

void CSsGameApp::OnTimer60()
{
}

void CSsGameApp::OnTimer300()
{
}

BOOL CSsGameApp::OnReload()
{
	return TRUE;
}

BOOL CSsGameApp::OnConnect(const LINKBASE* info)
{
	(void)info;
	return TRUE;
}

BOOL CSsGameApp::OnDisConnect(const LINKBASE* info)
{
	(void)info;
	return TRUE;
}

BOOL CSsGameApp::OnConnectAll()
{
	return TRUE;
}

int CSsGameApp::__OnInit(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnInit() ? 0 : -1;
}

int CSsGameApp::__OnDestroy(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnDestroy() ? 0 : -1;
}

int CSsGameApp::__OnQuit(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnQuit() ? -1 : 0;
}

int CSsGameApp::__OnFini(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnFini() ? 0 : -1;
}

int CSsGameApp::__OnProc(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnProc() ? 0 : -1;
}

int CSsGameApp::__OnTick(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnTick() ? 0 : -1;
}

int CSsGameApp::__OnReload(TAPPCTX* ctx, void* args)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnReload() ? 0 : -1;
}

int CSsGameApp::__OnConnect(TAPPCTX* ctx, void* args, const LINKBASE* info)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnConnect(info) ? 0 : -1;
}

int CSsGameApp::__OnDisconnect(TAPPCTX* ctx, void* args, const LINKBASE* info)
{
	(void)ctx;
	CSsGameApp* app = static_cast<CSsGameApp*>(args);
	return app && app->_OnDisconnect(info) ? 0 : -1;
}

BOOL CSsGameApp::_OnInit()
{
	BOOL bResume = m_Ctx.iStartMode == TAPP_START_MODE_RESUME;
	time_t now = time(NULL);
	m_RealTime = now;
	m_RealTimestamp = static_cast<uint64_t>(now);
	m_StartTick = static_cast<uint64_t>(clock());
	m_RealTick = m_StartTick;

#if defined(_WIN32) || defined(_WIN64)
	localtime_s(&m_CurLocalTime, &now);
#else
	localtime_r(&now, &m_CurLocalTime);
#endif

	if (!OnIniting(bResume))
	{
		OnInitFail();
		return FALSE;
	}

	if (!GetCfg().Load(m_Ctx.pszConfFile ? m_Ctx.pszConfFile : ""))
	{
		OnInitFail();
		return FALSE;
	}

	if (m_Ctx.pszConfFile && strlen(m_Ctx.pszConfFile) > 0)
	{
		int32_t nRetCode = tapp_network_init();
		if (nRetCode != errSuccess && nRetCode != AGERR_SUCCESS)
		{
			OnInitFail();
			return FALSE;
		}
		m_NetworkStarted = TRUE;
	}

	if (!OnInit(bResume))
	{
		OnInitFail();
		return FALSE;
	}
	return TRUE;
}

BOOL CSsGameApp::_OnDestroy()
{
	return OnDestroy();
}

BOOL CSsGameApp::_OnQuit()
{
	if (!m_bStopFlag)
	{
		return FALSE;
	}
	m_Quit = TRUE;
	return OnQuit();
}

BOOL CSsGameApp::_OnFini()
{
	if (m_NetworkStarted)
	{
		tapp_network_uninit();
		m_NetworkStarted = FALSE;
	}
	OnFini();
	OnDestroyed();
	return TRUE;
}

BOOL CSsGameApp::_OnProc()
{
	if (m_NetworkStarted)
	{
		tapp_network_run();
	}
	OnProc();
	return TRUE;
}

BOOL CSsGameApp::_OnTick()
{
	++m_Ctx.iTickCount;
	++m_CurFrame;
	m_RealTick = static_cast<uint64_t>(clock());
	m_RealTime = time(NULL);
	m_RealTimestamp = static_cast<uint64_t>(m_RealTime);

#if defined(_WIN32) || defined(_WIN64)
	localtime_s(&m_CurLocalTime, &m_RealTime);
#else
	localtime_r(&m_RealTime, &m_CurLocalTime);
#endif

	OnTimer1();
	if ((m_CurFrame % 10) == 0)
	{
		OnTimer10();
	}
	if ((m_CurFrame % 60) == 0)
	{
		OnTimer60();
	}
	if ((m_CurFrame % 300) == 0)
	{
		OnTimer300();
	}
	OnFrame(m_CurFrame);
	return OnTick();
}

BOOL CSsGameApp::_OnReload()
{
	return OnReload();
}

BOOL CSsGameApp::_OnConnect(const LINKBASE* info)
{
	return OnConnect(info);
}

BOOL CSsGameApp::_OnDisconnect(const LINKBASE* info)
{
	return OnDisConnect(info);
}

