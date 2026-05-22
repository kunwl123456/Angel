#include "stdafx.h"

#include "game_data/role_data.h"

#include <time.h>

CRoleData::CRoleData()
	: roleId(0),
	  accountId(0),
	  level(1),
	  lastSaveTime(0),
	  dirty(FALSE)
{
}

BOOL CRoleDataCache::Init()
{
	return TRUE;
}

BOOL CRoleDataCache::LoadOrCreateRole(uint64_t roleId, uint64_t accountId, const std::string& name, CRoleData** outRole)
{
	if (roleId == 0)
	{
		return FALSE;
	}

	CRoleData& role = m_Roles[roleId];
	if (role.roleId == 0)
	{
		role.roleId = roleId;
		role.accountId = accountId;
		role.name = name;
		role.level = 1;
		role.lastSaveTime = 0;
		role.dirty = TRUE;
	}

	if (outRole)
	{
		*outRole = &role;
	}
	return TRUE;
}

CRoleData* CRoleDataCache::FindRole(uint64_t roleId)
{
	std::map<uint64_t, CRoleData>::iterator it = m_Roles.find(roleId);
	return it == m_Roles.end() ? NULL : &it->second;
}

BOOL CRoleDataCache::MarkDirty(uint64_t roleId)
{
	CRoleData* role = FindRole(roleId);
	if (!role)
	{
		return FALSE;
	}
	role->dirty = TRUE;
	return TRUE;
}

BOOL CRoleDataCache::FlushDirtyRoles(std::vector<CRoleData>* flushedRoles)
{
	const uint64_t now = static_cast<uint64_t>(time(NULL));
	for (std::map<uint64_t, CRoleData>::iterator it = m_Roles.begin(); it != m_Roles.end(); ++it)
	{
		CRoleData& role = it->second;
		if (!role.dirty)
		{
			continue;
		}
		role.lastSaveTime = now;
		role.dirty = FALSE;
		if (flushedRoles)
		{
			flushedRoles->push_back(role);
		}
	}
	return TRUE;
}
