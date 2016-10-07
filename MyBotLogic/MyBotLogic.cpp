#include "MyBotLogic.h"
#include "Graph.h"
#include "Heuristic.h"
#include "Node.h"
#include "TurnInfo.h"

#include <iostream>
#include "windows.h"

MyBotLogic::MyBotLogic()
{
	//Write Code Here
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
	//Write Code Here
    Sleep(9000);
    Graph::Instance().init(_levelInfo);
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
        Graph::Instance().createGraph(_turnInfo);

    for (auto& npc : _turnInfo.npcs)
    {
        //1. Compute nearestTargets
        unsigned int npc1GoalID{ 3 };   //map TC_004
        unsigned int npc2GoalID{ 48 };

        //2. Compute A* to get opti path - if you finally manage to make it fucking work...
        std::list<unsigned int> pathTileID = pathFinderAStar(Graph::Instance(), npc.second.tileID, npc1GoalID, Heuristic(Graph::Instance().getNode(npc1GoalID)));

        //3. Get direction to next tile


        //4. If not blocked, push Action : Move to next tile
        //   Else if : first turn blocked, wait for it...
        //   Else :    target is already occupied, compute new path
        

    }
}

/*virtual*/ void MyBotLogic::Exit()
{
	//Write Code Here
}


std::list<unsigned int> MyBotLogic::pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic& h)
{

    Node* start{ graph.getNode(startID) };
    Node* goal{ graph.getNode(goalID) };

    //Init record
    NodeRecord* currentRecord = new NodeRecord{ start, 0, h.estimate(start) };

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
            if (!neighbour || isForbidden(neighbour))
                continue;

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
                endNodeRecord = new NodeRecord{ neighbour, 0, h.estimate(neighbour) };
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

bool MyBotLogic::isForbidden(Node* node)
{
    if (!node)
        return false;

    return std::find(node->getTileAttributes().begin(),
        node->getTileAttributes().end(),
        TileAttribute_Forbidden)
        != node->getTileAttributes().end();
}