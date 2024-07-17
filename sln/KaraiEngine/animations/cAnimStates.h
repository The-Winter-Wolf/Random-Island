#pragma once

#include <string>

enum eAnimStates
{
	idle,

	run_left,
	run_right,
	run_forward,
	run_backward,

	walk_left,
	walk_right,
	walk_forward,
	walk_backward,

	jump_run,
	jump_idle,

	kick,
	slash,
	boxing,
	power_up,


};

class cAnimStates
{
public:
	cAnimStates();
	~cAnimStates();

	float GetState(const eAnimStates& goalState, const eAnimStates& pastState, eAnimStates& nextState);
};

