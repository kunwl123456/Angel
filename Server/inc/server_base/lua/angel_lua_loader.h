#ifndef ANGEL_LUA_LOADER_H_
#define ANGEL_LUA_LOADER_H_

#include <string>
#include <vector>

#include "base_lib/def/type_def.h"

#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "luaintf/LuaIntf.h"
#endif

class CAngelLuaLoader
{
public:
	CAngelLuaLoader();
	virtual ~CAngelLuaLoader();

	BOOL Init(const std::string& scriptRoot);
	BOOL LoadScriptFile(const std::string& file);
	BOOL ExecuteString(const std::string& code);
	BOOL ReloadAllLuaFiles();
	BOOL ReloadLuaFiles(const std::vector<std::string>& files);
	BOOL RunGmFunction(const std::string& luaFunc, const std::vector<std::string>& args);

	const std::string& GetScriptRoot() const { return m_ScriptRoot; }
	const std::vector<std::string>& GetLoadedFiles() const { return m_LoadedFiles; }

private:
	std::string ResolveScriptPath(const std::string& file) const;

private:
	std::string m_ScriptRoot;
	std::vector<std::string> m_LoadedFiles;
#ifdef ANGEL_WITH_NFSHMXFRAME_LUABIND
	// LuaBind/LuaIntf is provided by NFShmXFrame. Angel keeps the runtime
	// optional at this layer so existing C++ services can boot before gameplay
	// scripts are migrated.
	LuaIntf::LuaContext* m_LuaContext;
#endif
};

class CAngelLuaScriptModule
{
public:
	CAngelLuaScriptModule();
	virtual ~CAngelLuaScriptModule();

	BOOL Init(const std::string& scriptRoot);
	BOOL Tick();
	BOOL Shut();

	void SessionReport(uint64_t playerId, const std::string& report);
	void SessionClose(uint64_t playerId);
	void ReloadAllLuaFiles();
	void ReloadLuaFiles(const std::vector<std::string>& files);
	void RunGmFunction(const std::string& luaFunc, const std::vector<std::string>& args);

	CAngelLuaLoader& GetLoader() { return m_Loader; }

private:
	CAngelLuaLoader m_Loader;
};

#endif // ANGEL_LUA_LOADER_H_
