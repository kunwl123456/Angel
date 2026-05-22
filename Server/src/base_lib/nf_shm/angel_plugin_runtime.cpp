#include "stdafx.h"

#include "base_lib/nf_shm/angel_plugin_runtime.h"

CAngelPluginRuntime& CAngelPluginRuntime::Instance()
{
	static CAngelPluginRuntime s_Runtime;
	return s_Runtime;
}

BOOL CAngelPluginRuntime::RegisterPlugin(CAngelPlugin* plugin)
{
	if (!plugin)
	{
		return FALSE;
	}

	for (size_t i = 0; i < m_Plugins.size(); ++i)
	{
		if (m_Plugins[i] == plugin)
		{
			return TRUE;
		}
	}

	m_Plugins.push_back(plugin);
	return TRUE;
}

BOOL CAngelPluginRuntime::Install()
{
	for (size_t i = 0; i < m_Plugins.size(); ++i)
	{
		if (!m_Plugins[i]->Install())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelPluginRuntime::Awake()
{
	for (size_t i = 0; i < m_Plugins.size(); ++i)
	{
		if (!m_Plugins[i]->Awake())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelPluginRuntime::Init()
{
	for (size_t i = 0; i < m_Plugins.size(); ++i)
	{
		if (!m_Plugins[i]->Init())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelPluginRuntime::Tick()
{
	for (size_t i = 0; i < m_Plugins.size(); ++i)
	{
		if (!m_Plugins[i]->Tick())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelPluginRuntime::Shut()
{
	for (std::vector<CAngelPlugin*>::reverse_iterator it = m_Plugins.rbegin(); it != m_Plugins.rend(); ++it)
	{
		if (!(*it)->Shut())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAngelPluginRuntime::Finalize()
{
	for (std::vector<CAngelPlugin*>::reverse_iterator it = m_Plugins.rbegin(); it != m_Plugins.rend(); ++it)
	{
		if (!(*it)->Finalize())
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CAngelPluginRuntime::Clear()
{
	m_Plugins.clear();
}
