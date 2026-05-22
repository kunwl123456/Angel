#ifndef ANGEL_PLUGIN_RUNTIME_H_
#define ANGEL_PLUGIN_RUNTIME_H_

#include <string>
#include <vector>

#include "base_lib/def/type_def.h"

class CAngelPlugin
{
public:
	virtual ~CAngelPlugin() {}

	virtual const char* GetPluginName() const = 0;
	virtual BOOL Install() { return TRUE; }
	virtual BOOL Awake() { return TRUE; }
	virtual BOOL Init() { return TRUE; }
	virtual BOOL Tick() { return TRUE; }
	virtual BOOL Shut() { return TRUE; }
	virtual BOOL Finalize() { return TRUE; }
};

class CAngelPluginRuntime
{
public:
	static CAngelPluginRuntime& Instance();

	BOOL RegisterPlugin(CAngelPlugin* plugin);
	BOOL Install();
	BOOL Awake();
	BOOL Init();
	BOOL Tick();
	BOOL Shut();
	BOOL Finalize();
	void Clear();

	const std::vector<CAngelPlugin*>& GetPlugins() const { return m_Plugins; }

private:
	CAngelPluginRuntime() {}
	CAngelPluginRuntime(const CAngelPluginRuntime&);
	CAngelPluginRuntime& operator=(const CAngelPluginRuntime&);

private:
	std::vector<CAngelPlugin*> m_Plugins;
};

#endif // ANGEL_PLUGIN_RUNTIME_H_
