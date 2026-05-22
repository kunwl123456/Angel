#include "stdafx.h"

#include "server_base/lua/angel_lua_loader.h"

#include <stdio.h>

CAngelLuaLoader::CAngelLuaLoader()
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	: m_LuaContext(NULL)
#endif
{
}

CAngelLuaLoader::~CAngelLuaLoader()
{
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	delete m_LuaContext;
	m_LuaContext = NULL;
#endif
}

BOOL CAngelLuaLoader::Init(const std::string& scriptRoot)
{
	m_ScriptRoot = scriptRoot;
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	if (!m_LuaContext)
	{
		m_LuaContext = new LuaIntf::LuaContext();
	}
#endif
	return TRUE;
}

BOOL CAngelLuaLoader::ReloadAllLuaFiles()
{
	// Angel keeps this module as the integration point for NFShmXFrame's
	// LuaBind/LuaIntf stack. The first migration stage records the runtime
	// contract and keeps script reload safe even before gameplay scripts exist.
	m_LoadedFiles.clear();
	return TRUE;
}

BOOL CAngelLuaLoader::ReloadLuaFiles(const std::vector<std::string>& files)
{
	for (size_t i = 0; i < files.size(); ++i)
	{
		if (!LoadFile(files[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelLuaLoader::RunGmFunction(const std::string& luaFunc, const std::vector<std::string>& args)
{
	(void)args;
	return !luaFunc.empty();
}

BOOL CAngelLuaLoader::LoadFile(const std::string& file)
{
	if (file.empty())
	{
		return FALSE;
	}
	m_LoadedFiles.push_back(file);
	return TRUE;
}

CAngelLuaScriptModule::CAngelLuaScriptModule()
{
}

CAngelLuaScriptModule::~CAngelLuaScriptModule()
{
}

BOOL CAngelLuaScriptModule::Init(const std::string& scriptRoot)
{
	return m_Loader.Init(scriptRoot);
}

BOOL CAngelLuaScriptModule::Tick()
{
	return TRUE;
}

BOOL CAngelLuaScriptModule::Shut()
{
	return TRUE;
}

void CAngelLuaScriptModule::SessionReport(uint64_t playerId, const std::string& report)
{
	(void)playerId;
	(void)report;
}

void CAngelLuaScriptModule::SessionClose(uint64_t playerId)
{
	(void)playerId;
}

void CAngelLuaScriptModule::ReloadAllLuaFiles()
{
	m_Loader.ReloadAllLuaFiles();
}

void CAngelLuaScriptModule::ReloadLuaFiles(const std::vector<std::string>& files)
{
	m_Loader.ReloadLuaFiles(files);
}

void CAngelLuaScriptModule::RunGmFunction(const std::string& luaFunc, const std::vector<std::string>& args)
{
	m_Loader.RunGmFunction(luaFunc, args);
}
