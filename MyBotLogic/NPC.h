#ifndef NPC_H
#define NPC_H

#include "MyBotLogic.h"
#include "Heuristic.h"
#include "BehaviorTree\Blackboard.h"

#include <list>

class IBehaviour;
struct NPCInfo;

class NPC
{
public:
	using cost_type = MyBotLogic::cost_type;
	using distance_id_pair_type = MyBotLogic::distance_id_pair_type;

private:
	NPCInfo mInfos;
	unsigned int mNextTileID;
	unsigned int mNbTurnBlocked;

	distance_id_pair_type mNearestTargets;  //Sorted targets ID from nearest to furthest
	std::list<unsigned int> mPathToGoal;    //Path of Tile IDs to goal

	Blackboard mBoard;
	IBehaviour *mBehavior;

public:
	NPC(const NPCInfo& infos, const distance_id_pair_type& nearest, LevelInfo& lvlInfo)
		: mNearestTargets{ nearest }, mNextTileID{ infos.tileID }, mNbTurnBlocked{ 0 }, mPathToGoal{}
	{
		mInfos = infos;
		initPath();
		initBehaviour(lvlInfo);
	}
	~NPC() { delete mBehavior; }

	void update(std::vector<Action*>& actionList);
	void updateInfos(const NPCInfo& npcInfo, const std::vector<NPC*>& npcs);

	void wait() {  ++mNbTurnBlocked; }
	void findNewPath();

	std::list<unsigned int> getPathToGoal() const { return mPathToGoal; }
	unsigned int getNbTurnWaited() const { return mNbTurnBlocked; }
	unsigned int getNextTileID() const { return mNextTileID; }
	NPCInfo getInfos() const { return mInfos; }


private:
	void initBehaviour(LevelInfo& lvlInfo);
	void initPath();

	EDirection chooseDirection(const unsigned int&, const unsigned int&);
	void move(std::vector<Action *> &_actionList);
	std::list<unsigned int> pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic&);

	void updatePathToGoal()
	{
		mNextTileID = mPathToGoal.front();
		mPathToGoal.pop_front();
	}

	Blackboard getBlackboard() const { return mBoard; }
	NPC::distance_id_pair_type getNearestTargets() const { return mNearestTargets; }
	void setNearestTargets(NPC::distance_id_pair_type val) { mNearestTargets = val; }
};

#endif //!NPC_H
