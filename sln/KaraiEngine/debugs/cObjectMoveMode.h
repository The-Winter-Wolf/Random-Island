#pragma once

#include "../global/global.h"
#include "../objects/object_3D/cObject3D.h"

#if OBJECT_CHANGE_MODE

enum ObjectEvents
{
	OBJECT_MOVE,
	OBJECT_SELECT,
	OBJECT_BOOST,

	OBJECT_FORWARD,
	OBJECT_BACKWARD,
	OBJECT_RIGHT,
	OBJECT_LEFT,
	OBJECT_UP,
	OBJECT_DOWN,

	OBJECT_ROTATE_FX,
	OBJECT_ROTATE_BX,
	OBJECT_ROTATE_FY,
	OBJECT_ROTATE_BY,
	OBJECT_ROTATE_FZ,
	OBJECT_ROTATE_BZ,

	OBJECT_SCALE
};

class cObjectMoveMode
{
public:
	cObjectMoveMode();
	~cObjectMoveMode();

	void MoveModel(ObjectEvents event, float eventValue);

private:

	cObject3D* m_pObject;
	cPhysicsBody* m_pPhysObject;

	bool bHasLight;
	bool bHasColor;
	glm::vec4 colorOfModel;

	float moveSpeed;
	float firstSpeed;
	float secondSpeed;

	float m_scale;

	float mass;
	glm::vec3 lastPos;
};

#endif // OBJECT_CHANGE_MODE

