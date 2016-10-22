#include "OmniscientBehaviour.h"
#include "MyBotLogic/BehaviorTree/Blackboard.h"
#include "MyBotLogic/Graph.h"
#include "MyBotLogic/Node.h"

void OmniscientBehaviour::update(Blackboard& bboard, NPC* npc)
{
	if (isArrived(*npc))
	{
		return;
	}

	if (!isNotBlocked(*npc))
	{
		npc->wait();
		if (npc->getNbTurnWaited() == 1)
		{
			return;
		}
		else if (npc->getNbTurnWaited() == 2)
		{
			Graph::Instance().getNode(npc->getPathToGoal().front())->taken() = true;
			npc->findNewPath();
		}
	}

	//move(actionList);
}

bool OmniscientBehaviour::isArrived(const NPC& npc) const
{
	return npc.getPathToGoal().empty();
}

bool OmniscientBehaviour::isNotBlocked(const NPC& npc) const
{
	if (Graph::Instance().getNode(npc.getPathToGoal().front())->taken())
	{
		return false;
	}
	return true;
}
