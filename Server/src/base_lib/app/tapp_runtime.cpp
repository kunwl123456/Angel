#include "stdafx.h"

#include "app/tapp_runtime.h"
#include "base_lib/wbus.h"

#include <string>

TAPPCTX g_tapp_ctx;
TAPPCTX* g_app_ctx = NULL;
void* g_app_arg = NULL;
AGBUSLOCALINFO g_local_info;

namespace
{
    bool g_quit_requested = false;
    std::string g_app_name;
    std::string g_conf_file;
    std::string g_bus_id;

    const char* get_arg_value(const char* arg, const char* prefix)
    {
        size_t len = strlen(prefix);
        if (strncmp(arg, prefix, len) != 0)
        {
            return NULL;
        }
        return arg + len;
    }
}

int tapp_def_init(TAPPCTX* ctx, int argc, char* argv[], const char* app_name, void* app_arg)
{
    if (!ctx)
    {
        return -1;
    }

    memset(ctx, 0, sizeof(TAPPCTX));
    memset(&g_local_info, 0, sizeof(g_local_info));

    g_app_name = app_name ? app_name : "angel_server";
    g_conf_file.clear();
    g_bus_id.clear();

    ctx->argc = argc;
    ctx->argv = argv;
    ctx->pszApp = g_app_name.c_str();
    ctx->iTimer = 10;
    ctx->iEpollWait = 10;
    ctx->iIdleCount = 100;
    ctx->iIdleSleep = 1;
    ctx->iStartMode = TAPP_START_MODE_INIT;
    ctx->iUseBus = 1;

    for (int i = 1; i < argc; ++i)
    {
        const char* conf = get_arg_value(argv[i], "--conf=");
        if (conf)
        {
            g_conf_file = conf;
            continue;
        }

        const char* id = get_arg_value(argv[i], "--id=");
        if (id)
        {
            g_bus_id = id;
            wbus_addr_aton(g_bus_id.c_str(), &ctx->iId);
            continue;
        }

        const char* timer = get_arg_value(argv[i], "--timer=");
        if (timer)
        {
            ctx->iTimer = atoi(timer);
            if (ctx->iTimer <= 0)
            {
                ctx->iTimer = 10;
            }
        }
    }

    if (!g_conf_file.empty())
    {
        ctx->pszConfFile = g_conf_file.c_str();
    }
    if (!g_bus_id.empty())
    {
        ctx->pszId = g_bus_id.c_str();
    }

    g_app_ctx = ctx;
    g_app_arg = app_arg;
    g_quit_requested = false;
    return 0;
}

int tapp_mainloop(TAPPCTX* ctx)
{
    if (!ctx)
    {
        return -1;
    }

    if (ctx->pfnInit && ctx->pfnInit(ctx, g_app_arg) != 0)
    {
        return -1;
    }

    while (!g_quit_requested)
    {
        if (ctx->pfnProc)
        {
            ctx->pfnProc(ctx, g_app_arg);
        }
        if (ctx->pfnTick && ctx->pfnTick(ctx, g_app_arg) != 0)
        {
            break;
        }
        if (ctx->pfnQuit && ctx->pfnQuit(ctx, g_app_arg) != 0)
        {
            break;
        }

#if defined(_WIN32) || defined(_WIN64)
        Sleep(ctx->iTimer);
#else
        usleep(ctx->iTimer * 1000);
#endif
    }

    if (ctx->pfnFini)
    {
        ctx->pfnFini(ctx, g_app_arg);
    }

    return 0;
}

void tapp_request_quit()
{
    g_quit_requested = true;
}
