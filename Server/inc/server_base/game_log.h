#ifndef _H_GAME_LOG_H__
#define _H_GAME_LOG_H__

class CGameLog
{
public:
	CGameLog();
	~CGameLog();

	void Info(const char* fmt, ...);
	void Error(const char* fmt, ...);
};

#endif // _H_GAME_LOG_H__
