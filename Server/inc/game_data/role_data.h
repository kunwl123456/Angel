#ifndef ANGEL_ROLE_DATA_H_
#define ANGEL_ROLE_DATA_H_

#include <map>
#include <string>
#include <vector>

#include "base_lib/def/type_def.h"

struct CRoleData
{
	CRoleData();

	uint64_t roleId;
	uint64_t accountId;
	std::string name;
	uint32_t level;
	uint64_t lastSaveTime;
	BOOL dirty;
};

class CRoleDataCache
{
public:
	BOOL Init();
	BOOL LoadOrCreateRole(uint64_t roleId, uint64_t accountId, const std::string& name, CRoleData** outRole);
	CRoleData* FindRole(uint64_t roleId);
	BOOL MarkDirty(uint64_t roleId);
	BOOL FlushDirtyRoles(std::vector<CRoleData>* flushedRoles = NULL);
	size_t GetRoleCount() const { return m_Roles.size(); }

private:
	std::map<uint64_t, CRoleData> m_Roles;
};

#endif // ANGEL_ROLE_DATA_H_
