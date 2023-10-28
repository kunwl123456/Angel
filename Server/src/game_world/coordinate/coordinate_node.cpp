#include "stdafx.h"
#include "game_world/coordinate_node.h"
#include "game_world/game_world.h"
#include "game_world/coordinate_agent.h"
#include "game_world/coordinate_system.h"

#include "base_lib/linked_list.h"
#include "base_lib/process_error.h"

/*

AGCoordNode* coordinate_node_alloc()
{
	//����ѩ���㷨���ɶ�ӦobjID
	AGCoordNode* p = NULL;

	new(p) AGCoordNode;

	int ret = p->init();
	assert_retval(ret == 0, NULL);

	p->nUid = 0;//������ѩ�����ɵ�objID

	return p;
}

bool coordinate_node_free(AGCoordNode* node)
{
	assert_retval(node, false);
	int ret = node->fini();
	assert_retval(ret == 0, ret);

	return true;//����Ӧ����game_world�����ɾ���������
}

int AGCoordNode::init()
{
	int ret = cur_pos.init();
	assert_retval(ret == 0,ret);

	ret = cur_rad.init();
	assert_retval(ret == 0, ret);

	ret = old_pos.init();
	assert_retval(ret == 0, ret);

	ret = old_rad.init();
	assert_retval(ret == 0, ret);

	xPrev = NULL;
	xNext = NULL;
#if AG_COORD_HAS_Y
	yPrev = NULL;
	yNext = NULL;
#endif

	zNext = NULL;
	zPrev = NULL;

	lGuard = NULL;
	rGuard = NULL;

	oWner = NULL;
	//agent = NULL;
	nUid = 0;
	ntype = agcNodeMin;
	install = false;
	is_updated = false;
	return 0;
}

int AGCoordNode::fini()
{
	int ret = cur_pos.fini();
	assert_retval(ret == 0, ret);

	ret = cur_rad.fini();
	assert_retval(ret == 0, ret);

	ret = old_pos.fini();
	assert_retval(ret == 0, ret);

	ret = old_rad.fini();
	assert_retval(ret == 0, ret);

	return 0;
}


*/







