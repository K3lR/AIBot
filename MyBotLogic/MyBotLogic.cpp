#include "MyBotLogic.h"
#include "Graph.h"
#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"
#include "NPC.h"
#include "NPCInfo.h"
#include "TurnInfo.h"

#include <cassert>
#include "windows.h"

MyBotLogic::MyBotLogic()
	: mNPCs{}
{
}

/*virtual*/ MyBotLogic::~MyBotLogic()
{
	//Write Code Here
}

/*virtual*/ void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);


	//Write Code Here
}

/*virtual*/ void MyBotLogic::Load()
{
	//Write Code Here
}

/*virtual*/ void MyBotLogic::Init(LevelInfo& _levelInfo)
{
	Sleep(8000);

	Graph::Instance().init(_levelInfo);
	initNpcs(_levelInfo);
}

/*virtual*/ void MyBotLogic::OnBotInitialized()
{
	//Write Code Here
}

/*virtual*/ void MyBotLogic::OnGameStarted()
{
	//Write Code Here
}

/*virtual*/ void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{

	updateTurn(_turnInfo);

	for (auto& npc : mNPCs)
	{
		npc->update(_actionList);
	}
}

/*virtual*/ void MyBotLogic::Exit()
{
	//Write Code Here
}


MyBotLogic::distance_id_pair_type MyBotLogic::findNearestTargetsByNPC(const std::vector<Node*>& graph, const NPCInfo& npc)
{
	//Set new heuristic with node corresponding to NPC's tile
	Heuristic h{ graph[npc.tileID] };
	distance_id_pair_type distancesToTargets;
	for (Node* target : Graph::Instance().getTargetList())
	{
		distancesToTargets.emplace(std::make_pair(h.estimate(target, Graph::Instance().getLevelInfo()), target->getID()));
	}

	return distancesToTargets;
}

void MyBotLogic::initNpcs(LevelInfo& lvlInfo)
{
	for (auto& npcInfoPair : Graph::Instance().getLevelInfo().npcs)
	{
		distance_id_pair_type nearestTargets = findNearestTargetsByNPC(Graph::Instance().getGraph(), npcInfoPair.second);
		mNPCs.emplace_back(new NPC{ npcInfoPair.second, nearestTargets, lvlInfo });
	}
}

void MyBotLogic::updateTurn(const TurnInfo& turnInfo)
{
	int i{ 0 };
	for (auto& npcInfoPair : turnInfo.npcs)
	{
		mNPCs[i]->updateInfos(npcInfoPair.second, mNPCs);
		++i;
	}
}