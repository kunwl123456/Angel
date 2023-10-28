#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <map>
#include "def/type_def.h"

class CConfigFile
{
private:
	CConfigFile(void);
	~CConfigFile(void);

public:
	static CConfigFile* GetInstancePtr();

public:
	BOOL Load(std::string strFileName);

	std::string GetStringValue(std::string strName);

	int32_t GetIntValue(std::string VarName);

	float GetFloatValue(std::string VarName);

	double GetDoubleValue(std::string VarName);

	int32_t  GetRealNetPort(std::string VarName);

private:
	std::map<std::string, std::string> m_Values;
};

#endif