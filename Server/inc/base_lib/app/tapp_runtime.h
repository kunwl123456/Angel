#ifndef TAPP_RUNTIME_H
#define TAPP_RUNTIME_H

#include "app/tapp.h"
#include "base_lib/wbus_def.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TAPPCTX g_tapp_ctx;
extern TAPPCTX* g_app_ctx;
extern void* g_app_arg;
extern AGBUSLOCALINFO g_local_info;

int tapp_def_init(TAPPCTX* ctx, int argc, char* argv[], const char* app_name, void* app_arg);
int tapp_mainloop(TAPPCTX* ctx);
void tapp_request_quit();

#ifdef __cplusplus
}
#endif

#endif // TAPP_RUNTIME_H
