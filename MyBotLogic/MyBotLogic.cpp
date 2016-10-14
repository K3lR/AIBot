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
    : mNPCs{}, mInvColCount{}
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

    mLevelInfo = _levelInfo;
    mInvColCount = 1.f / mLevelInfo.colCount;

    //set VisionType
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
    if (Graph::Instance().isEmpty())
    {
        Graph::Instance().createGraph(_turnInfo, mLevelInfo);

        initNpcs(_turnInfo);
    }
    else updateTurn(_turnInfo);

    for (auto& npc : mNPCs)
    {
        if (npc.isArrived())
        {
            //setNextState(ARRIVED);
            continue;
        }

        if (!npc.isNotBlocked(mNPCs))
        {
            ++npc.mNbTurnBlocked;
            if (npc.mNbTurnBlocked == 1 /*npc.hasToWait()*/)
            {
                //setNextState(WAITING);
                continue;
            }
            else if (npc.mNbTurnBlocked == 2 /*npc.willBeBlocked()*/)
            {
                //setNextState(BLOCKED);
                Graph::Instance().getNode(npc.mPathToGoal.front())->insertTileAttribute(TileAttribute_Obstacle);
                findNewPath(npc);
            }
        }
        //else setNextState(MOVING);
        //npc.update(mNPCs);

        moveNPC(npc, _actionList);
    }
}

/*virtual*/ void MyBotLogic::Exit()
{
    //Write Code Here
}

//***Refactor : NPC ???
EDirection MyBotLogic::chooseDirection(const unsigned int& destinationTileID, const unsigned int& npcTileID)
{
    class Error_NoDirectionFound {};
    int delta = destinationTileID - npcTileID;

    //Forward-backward direction
    if (delta == 1)
        return E;
    if (delta == -1)
        return W;

    //NE, NW, SE, SW directions depend if NPC's on an even/odd row
    if (static_cast<unsigned int>(npcTileID * mInvColCount) % 2 != 0) //Even row, odd row index
    {
        //NE : delta == -(colCount - 1)
        if (delta == -(mLevelInfo.colCount - 1))
            return NE;

        //NW : delta == -colCount
        else if (delta == -mLevelInfo.colCount)
            return NW;

        //SE : delta == colCount + 1
        else if (delta == mLevelInfo.colCount + 1)
            return SE;

        //SW : delta == colCount
        else if (delta == mLevelInfo.colCount)
            return SW;

        //Problem...
        else throw Error_NoDirectionFound{};
    }
    else //Odd row, even row index
    {
        //NE : delta == -colCount
        if (delta == -mLevelInfo.colCount)
            return NE;

        //NW : delta == -(colCount + 1)
        if (delta == -(mLevelInfo.colCount + 1))
            return NW;

        //SE : delta == colCount
        if (delta == mLevelInfo.colCount)
            return SE;

        //SW : delta == colCount - 1
        if (delta == mLevelInfo.colCount - 1)
            return SW;

        else throw Error_NoDirectionFound{};
    }
}

MyBotLogic::distance_id_pair_type MyBotLogic::findNearestTargetsByNPC(const std::vector<Node*>& graph, const NPCInfo& npc)
{
    //Set new heuristic with node corresponding to NPC's tile
    Heuristic h{ graph[npc.tileID] };
    distance_id_pair_type distancesToTargets;
    for (Node* target : Graph::Instance().getTargetList())
    {
        distancesToTargets.emplace(std::make_pair(h.estimate(target, mLevelInfo), target->getID()));
    }

    return distancesToTargets;
}

//***Refactor : NPC
void MyBotLogic::findNewPath(NPC &npc)
{
    npc.mNearestTargets.erase(npc.mNearestTargets.begin());
    npc.mPathToGoal = pathFinderAStar(Graph::Instance(), npc.mInfos.tileID, npc.mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(npc.mNearestTargets.begin()->second)));
    npc.mNbTurnBlocked = 0;
}

void MyBotLogic::initNpcs(const TurnInfo& turnInfo)
{
    for (auto& npcInfoPair : turnInfo.npcs)
    {
        distance_id_pair_type nearestTargets = findNearestTargetsByNPC(Graph::Instance().getGraph(), npcInfoPair.second);
        
        //***Refactor : NPC
        std::list<unsigned int> pathToGoal = pathFinderAStar(Graph::Instance(), npcInfoPair.second.tileID, nearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(nearestTargets.begin()->second)));

        mNPCs.emplace_back(NPC{ npcInfoPair.second, nearestTargets, pathToGoal });
    }
}

//***Refactor : NPC
void MyBotLogic::moveNPC(NPC &npc, std::vector<Action *> &_actionList)
{
    EDirection dir = chooseDirection(npc.mPathToGoal.front(), npc.mInfos.tileID);
    _actionList.emplace_back(new Move{ npc.mInfos.npcID, dir });

    npc.updatePathToGoal();
}

//***Refactor : NPC
std::list<unsigned int> MyBotLogic::pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic& h)
{

    Node* start{ graph.getNode(startID) };
    Node* goal{ graph.getNode(goalID) };

    //Init record
    NodeRecord* currentRecord = new NodeRecord{ start, 0, h.estimate(start, mLevelInfo) };

    //Init lists
    std::list<NodeRecord*> openedList{}, closedList{};
    openedList.emplace_back(currentRecord);

    while (!openedList.empty())
    {

        //Get smallest element
        currentRecord = *std::min_element(std::begin(openedList), std::end(openedList));
        NodeRecord* prevRecord = new NodeRecord{};
        Node* currentNode{ currentRecord->mNode };

        //Found goal - yay!
        if (currentNode == goal)
            break;

        //If not, check neighbours to find smallest cost step
        NodeRecord* endNodeRecord = new NodeRecord{};
        for (auto& neighbour : currentNode->getNeighbours())
        {
            if (!neighbour || neighbour->containsAttribute(TileAttribute_Forbidden)
                || neighbour->containsAttribute(TileAttribute_Obstacle))
            {
                continue;
            }

            //Node* endNode = neighbour;
            cost_type endNodeCost = currentRecord->mCostSoFar + CONNECTION_COST;
            cost_type endNodeHeuristic{ 0 };

            //If closed node, may have to skip or remove it from closed list
            //**Remark : nRecord <-> endNodeRecord
            if ((endNodeRecord = NodeRecord::findIn(closedList, neighbour)))
            {
                //If not a shorter route, skip
                if (endNodeRecord->mCostSoFar <= endNodeCost)
                    continue;

                //If shorter, we need to put it back in the opened list
                closedList.remove(endNodeRecord);

                //Update heuristic
                endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
            }
            else if ((endNodeRecord = NodeRecord::findIn(openedList, neighbour)))
            {
                if (endNodeRecord->mCostSoFar <= endNodeCost)
                    continue;

                endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
            }
            //Unvited node : need a new record
            else
            {
                endNodeRecord = new NodeRecord{ neighbour, 0, h.estimate(neighbour, mLevelInfo) };
            }

            //Update record
            endNodeRecord->mCostSoFar = endNodeCost;
            endNodeRecord->mPrevious = currentRecord;
            endNodeRecord->mEstimatedTotalCost = endNodeCost + endNodeHeuristic;

            //Put record back in opened list
            openedList.emplace_back(endNodeRecord);

        }
        //Update previous
        prevRecord = currentRecord;

        //Looked at all current node's neighbours : remove opened, put it in closed
        openedList.remove(currentRecord);
        closedList.emplace_back(currentRecord);
    }

    //Found our goal or run out of nodes
    std::list<unsigned int> finalPath{};
    if (currentRecord->mNode != goal)
        return finalPath;

    else
    {
        //Work back along path
        while (currentRecord->mNode != start)
        {
            finalPath.emplace_front(currentRecord->mNode->getID());
            currentRecord = currentRecord->mPrevious;
        }
    }

    return finalPath;
}

void MyBotLogic::updateTurn(const TurnInfo& turnInfo)
{
    int i{ 0 };
    for (auto& npcInfoPair : turnInfo.npcs)
    {
        mNPCs[i].mInfos = npcInfoPair.second;
        ++i;
    }
}