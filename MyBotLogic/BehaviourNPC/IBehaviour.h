#ifndef I_BEHAVIOUR_H
#define I_BEHAVIOUR_H

#include "MyBotLogic\NPC.h"
#include "MyBotLogic\BehaviorTree\Blackboard.h"


class IBehaviour
{
public:
	virtual void update(Blackboard& bboard, NPC* npc) = 0;
};

#endif // I_BEHAVIOUR_H
