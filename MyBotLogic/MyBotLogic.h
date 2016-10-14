#pragma once

#include "BotLogicIF.h"
#include "Logger.h"

#include "Graph.h"
#include "LevelInfo.h"

#include <list>
#include <vector>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG
#endif

#ifdef BOT_LOGIC_DEBUG
#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif

struct CostCompare;
struct LevelInfo;
class NPC;
class Node;

//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{
public:
    MyBotLogic();
    virtual ~MyBotLogic();

    virtual void Configure(int argc, char *argv[], const std::string& _logpath);
    virtual void Load();
    virtual void OnBotInitialized();
    virtual void Init(LevelInfo& _levelInfo);
    virtual void OnGameStarted();
    virtual void FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList);   //calculate moves for a single turn
    virtual void Exit();

    /* Personal content */

    using cost_type = Graph::cost_type;
    using distance_id_pair_type = std::multimap<cost_type, unsigned int>;

    const cost_type CONNECTION_COST{ 10 };

    std::list<unsigned int> pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic&);

private:
    LevelInfo mLevelInfo;
    float mInvColCount;

    std::vector<NPC> mNPCs;

    EDirection chooseDirection(const unsigned int&, const unsigned int&);
    distance_id_pair_type findNearestTargetsByNPC(const std::vector<Node*>&, const NPCInfo&);
    void findNewPath(NPC &npc);
    void initNpcs(const TurnInfo&);
    void moveNPC(NPC &npc, std::vector<Action *> &_actionList);
    void updateTurn(const TurnInfo&);

protected:
    Logger mLogger;
};

