#ifndef NPC_H
#define NPC_H

#include <list>
#include "MyBotLogic.h"

struct NPCInfo;

class NPC
{
public:
    using cost_type = MyBotLogic::cost_type;
    using distance_id_pair_type = MyBotLogic::distance_id_pair_type;

private:
    enum EState { START, BLOCKED, MOVING, WAITING, ARRIVED };

    NPCInfo mInfos;
    unsigned int mNextTileID;
    unsigned int mNbTurnBlocked;

    distance_id_pair_type mNearestTargets;  //Sorted targets ID from nearest to furthest
    std::list<unsigned int> mPathToGoal;    //Path of Tile IDs to goal

public:
	NPC(const NPCInfo& infos, const distance_id_pair_type& nearest)
		: mNearestTargets{ nearest }, mNextTileID{ infos.tileID }, mNbTurnBlocked{ 0 }
	{
		mInfos = infos;
		mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID, mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(mNearestTargets.begin()->second)));
	}
	~NPC() {}

    bool isArrived()
    {
        return mPathToGoal.empty();
    }
    bool isNotBlocked(const std::vector<NPC>& npcs)
    {
        for (int i{}; i < npcs.size(); ++i)
        {
            if (i == mInfos.npcID - 1)
            {
                continue;
            }

            if (mPathToGoal.front() == npcs[i].mNextTileID)
            {
                return false;
            }
        }

        return true;
    }

	void update(std::vector<NPC>& npcs, std::vector<Action*>& actionList);
	void updateInfos(const NPCInfo& npcInfo);

private:
	EDirection chooseDirection(const unsigned int&, const unsigned int&);
	void move(std::vector<Action *> &_actionList);
	std::list<unsigned int> pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic&);
	void findNewPath();
    void updatePathToGoal()
    {
        mNextTileID = mPathToGoal.front();
        mPathToGoal.pop_front();
    }
};

#endif //!NPC8_H
