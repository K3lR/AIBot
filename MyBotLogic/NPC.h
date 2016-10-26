#ifndef NPC_H
#define NPC_H

#include <list>
#include "MyBotLogic.h"
#include <algorithm>

struct NPCInfo;

class NPC
{
public:
	using cost_type = MyBotLogic::cost_type;
	using distance_id_pair_type = MyBotLogic::distance_id_pair_type;
	using id_count_pair_type = std::map<unsigned int, unsigned int>;

private:
	enum EState { START, BLOCKED, MOVING, WAITING, ARRIVED };

	NPCInfo mInfos;
	unsigned int mNextTileID;
	unsigned int mNbTurnBlocked;

	int mGoalID;

	//***NOTE : no more used...for now
	distance_id_pair_type mNearestTargets;  //Sorted targets ID from nearest to furthest
	std::list<unsigned int> mPathToGoal;    //Path of Tile IDs to goal

	id_count_pair_type mExploredTiles;

public:
	NPC(const NPCInfo& infos, const distance_id_pair_type& nearest)
		: mPathToGoal{}, mNearestTargets{ nearest },
		mNextTileID{ infos.tileID }, mNbTurnBlocked{ 0 }, mGoalID{ -1 }
	{
		mInfos = infos;
		initPath(nearest);
	}
	NPC(const NPC& other)
	{
		mInfos = other.mInfos;
		mNearestTargets = other.mNearestTargets;
		mNextTileID = other.mNextTileID;
		mNbTurnBlocked = other.mNbTurnBlocked;
		mPathToGoal = other.mPathToGoal;
	}
	~NPC() {}

	bool isArrived()
	{
		return mPathToGoal.empty();
	}
	bool isBlocked()
	{
		if (Graph::Instance().getNode(mPathToGoal.front())->taken())
		{
			return true;
		}
		return false;
	}
	bool hasTargets() { return !mNearestTargets.empty(); }
	bool hasGoal() { return mGoalID > -1; }

	void update(std::vector<Action*>& actionList);
	void updateInfo(const NPCInfo& npcInfo);

	unsigned int getTileID();
	void setNearestTargets(distance_id_pair_type targets)
	{
		mNearestTargets = targets;
		mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID, mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(mNearestTargets.begin()->second)));
	}
	void setGoalID(unsigned int goalID)
	{
		mGoalID = goalID;
	}
	void setPath();

private:
	EDirection chooseDirection(const unsigned int&, const unsigned int&);
	void explore(std::vector<Action*>& actionList);

	std::vector<Node*> getAccessibleNeighbours(const std::vector<Node*> &neighbourTiles, Node * currNode);

	void findNewPath();

	void initPath(const distance_id_pair_type& nearest);
	void followPath(std::vector<Action *> &_actionList);

	unsigned int getLessVisitedTileId(std::vector<Node*> accessible_nodes)
	{
		unsigned int min_count = UINT_MAX;
		unsigned int minID{};
		for (int i{ 0 }; i < accessible_nodes.size(); ++i)
		{
			if (mExploredTiles[accessible_nodes[i]->getID()] == 0)
			{
				return accessible_nodes[i]->getID();
			}
			
			if (mExploredTiles[accessible_nodes[i]->getID()] < min_count)
			{
				minID = accessible_nodes[i]->getID();
				min_count = mExploredTiles[minID];
			}
		}
		return minID;
	}
	std::list<unsigned int> pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic&);
	void updatePathToGoal()
	{
		Graph::Instance().getNode(mInfos.tileID)->taken() = false;
		mNextTileID = mPathToGoal.front();
		mPathToGoal.pop_front();
		Graph::Instance().getNode(mNextTileID)->taken() = true;
	}
};

#endif //!NPC8_H
