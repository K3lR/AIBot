#ifndef STATE_H
#define STATE_H

#include "Globals.h"

#include <vector>

class State
{
	enum EState { START, BLOCKED, MOVING, WAITING, ARRIVED };

public:
	std::vector<Action*> getAction();


};

#endif //!STATE_H