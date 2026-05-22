#include "stdafx.h"

#include "server_base/lua/angel_lua_loader.h"

#include <fstream>
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

std::string CAngelLuaLoader::ResolveScriptPath(const std::string& file) const
{
	if (file.empty())
	{
		return file;
	}
	if (file.size() > 1 && (file[0] == '/' || file[1] == ':'))
	{
		return file;
	}
	if (m_ScriptRoot.empty())
	{
		return file;
	}
	const char tail = m_ScriptRoot[m_ScriptRoot.size() - 1];
	return m_ScriptRoot + (tail == '/' || tail == '\\' ? "" : "/") + file;
}

BOOL CAngelLuaLoader::LoadScriptFile(const std::string& file)
{
	if (file.empty())
	{
		return FALSE;
	}

	const std::string fullPath = ResolveScriptPath(file);
	std::ifstream input(fullPath.c_str());
	if (!input.good())
	{
		return FALSE;
	}

#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	try
	{
		m_LuaContext->doFile(fullPath.c_str());
	}
	catch (const LuaIntf::LuaException&)
	{
		return FALSE;
	}
#endif

	for (size_t i = 0; i < m_LoadedFiles.size(); ++i)
	{
		if (m_LoadedFiles[i] == file)
		{
			return TRUE;
		}
	}
	m_LoadedFiles.push_back(file);
	return TRUE;
}

BOOL CAngelLuaLoader::ExecuteString(const std::string& code)
{
	if (code.empty())
	{
		return FALSE;
	}
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	try
	{
		m_LuaContext->doString(code.c_str());
	}
	catch (const LuaIntf::LuaException&)
	{
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL CAngelLuaLoader::ReloadAllLuaFiles()
{
	const std::vector<std::string> files = m_LoadedFiles;
	m_LoadedFiles.clear();
	for (size_t i = 0; i < files.size(); ++i)
	{
		if (!LoadScriptFile(files[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelLuaLoader::ReloadLuaFiles(const std::vector<std::string>& files)
{
	for (size_t i = 0; i < files.size(); ++i)
	{
		if (!LoadScriptFile(files[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelLuaLoader::RunGmFunction(const std::string& luaFunc, const std::vector<std::string>& args)
{
	if (luaFunc.empty())
	{
		return FALSE;
	}
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	lua_State* L = m_LuaContext->state();
	lua_getglobal(L, luaFunc.c_str());
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return FALSE;
	}
	for (size_t i = 0; i < args.size(); ++i)
	{
		lua_pushstring(L, args[i].c_str());
	}
	if (lua_pcall(L, static_cast<int>(args.size()), 0, 0) != 0)
	{
		lua_pop(L, 1);
		return FALSE;
	}
#else
	(void)args;
#endif
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
