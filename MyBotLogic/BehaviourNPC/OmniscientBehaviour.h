#ifndef OMNISCIENT_BEHAVIOUR_H
#define OMNISCIENT_BEHAVIOUR_H

#include "IBehaviour.h"
#include "MyBotLogic\NPC.h"
#include "MyBotLogic\BehaviorTree\Blackboard.h"
#include <vector>

class OmniscientBehaviour : public IBehaviour
{

public:
	OmniscientBehaviour() {}
	~OmniscientBehaviour() {}
	virtual void update(Blackboard& bboard, NPC* npc) override;

private:
	bool isArrived(const NPC& npc) const;
	bool isNotBlocked(const NPC& npc) const;
};

#endif