#ifndef _SCENE_SERVER_H__
#define _SCENE_SERVER_H__

#include "server_base/ss_game_server_app.h"
#include "game_world/aoi_grid.h"

class CSceneServerApp : public CSsGameServerApp
{
public:
	CSceneServerApp();
	virtual const char* GetName() const override;

protected:
	virtual BOOL OnInit(BOOL bResume) override;
	virtual BOOL OnTick() override;

private:
	CAoiGrid m_AoiGrid;
};

#endif //_SCENE_SERVER_H__