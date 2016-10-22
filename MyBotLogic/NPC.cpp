#include "Heuristic.h"
#include "Node.h"
#include "NPC.h"
#include "Globals.h"
#include "BehaviorTree\Blackboard.h"
#include "BehaviourNPC\OmniscientBehaviour.h"

EDirection NPC::chooseDirection(const unsigned int& destinationTileID, const unsigned int& npcTileID)
{
	class Error_NoDirectionFound {};
	int delta = destinationTileID - npcTileID;

	//Forward-backward direction
	if (delta == 1)
		return E;
	if (delta == -1)
		return W;

	LevelInfo lvlInfo = Graph::Instance().getLevelInfo();
	//NE, NW, SE, SW directions depend if NPC's on an even/odd row
	if (static_cast<unsigned int>(npcTileID / lvlInfo.colCount) % 2 != 0) //Even row, odd row index
	{
		//NE : delta == -(colCount - 1)
		if (delta == -(lvlInfo.colCount - 1))
			return NE;

		//NW : delta == -colCount
		else if (delta == -lvlInfo.colCount)
			return NW;

		//SE : delta == colCount + 1
		else if (delta == lvlInfo.colCount + 1)
			return SE;

		//SW : delta == colCount
		else if (delta == lvlInfo.colCount)
			return SW;

		//Problem...
		else throw Error_NoDirectionFound{};
	}
	else //Odd row, even row index
	{
		//NE : delta == -colCount
		if (delta == -lvlInfo.colCount)
			return NE;

		//NW : delta == -(colCount + 1)
		if (delta == -(lvlInfo.colCount + 1))
			return NW;

		//SE : delta == colCount
		if (delta == lvlInfo.colCount)
			return SE;

		//SW : delta == colCount - 1
		if (delta == lvlInfo.colCount - 1)
			return SW;

		else throw Error_NoDirectionFound{};
	}
}

void NPC::findNewPath()
{
	mNearestTargets.erase(mNearestTargets.begin());
	mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID,mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(mNearestTargets.begin()->second)));
	mNbTurnBlocked = 0;
}

//***TODO : /!\ MEMO LEAK
std::list<unsigned int> NPC::pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic& h)
{

	Node* start{ graph.getNode(startID) };
	Node* goal{ graph.getNode(goalID) };

	//Init record
	NodeRecord* currentRecord = new NodeRecord{ start, 0, h.estimate(start, graph.getLevelInfo()) };

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
		for (int idNeighbour{}; idNeighbour < currentNode->getNeighbours().size(); ++idNeighbour)
		{
			Node* neighbour = currentNode->getNeighbour(idNeighbour);
			if (!neighbour || neighbour->isNotAvailable()
				           || currentNode->hasWall(idNeighbour))
			{
				continue;
			}

			cost_type endNodeCost = currentRecord->mCostSoFar + Graph::Instance().CONNECTION_COST;
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
				endNodeRecord = new NodeRecord{ neighbour, 0, h.estimate(neighbour, graph.getLevelInfo()) };
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

void NPC::move(std::vector<Action *> &_actionList)
{
	EDirection dir = chooseDirection(mPathToGoal.front(), mInfos.tileID);
	_actionList.emplace_back(new Move{ mInfos.npcID, dir });

	updatePathToGoal();
}

void NPC::initBehaviour(LevelInfo& lvlInfo)
{
	if (lvlInfo.bOmnicientMode)
	{
		mBehavior = new OmniscientBehaviour{};
	}
	else
	{
		//mBehavior = new  VisionBehavior(_levelInfo.visionRange);
	}
}

void NPC::initPath()
{
	if (!mNearestTargets.empty())
		mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID, mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(mNearestTargets.begin()->second)));
}

void NPC::update(std::vector<Action*>& actionList)
{
	mBehavior->update(mBoard, this);
	actionList.insert(actionList.end(), mBoard.getActionList().begin(), mBoard.getActionList().end());
}

//***TODO : reloacte in update()
void NPC::updateInfos(const NPCInfo& npcInfo, const std::vector<NPC*>& npcs)
{
	mInfos = npcInfo;
}