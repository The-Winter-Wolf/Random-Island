#include "cAnimStates.h"

cAnimStates::cAnimStates()
{
}

cAnimStates::~cAnimStates()
{
}

float cAnimStates::GetState(const eAnimStates& goalState, const eAnimStates& pastState, eAnimStates& nextState)
{
    if (goalState == jump_run || goalState == jump_idle)
    {
        if (pastState == run_left ||
            pastState == run_right ||
            pastState == run_forward ||
            pastState == run_backward)
        {
            nextState = jump_run;
            return 0.0f;
        }
        else if (pastState == idle)
        {
            nextState = jump_idle;
            return 0.0f;
        }
    }

    nextState = goalState;
    return 0.2f;
}
