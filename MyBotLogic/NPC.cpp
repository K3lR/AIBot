#include "Heuristic.h"
#include "Node.h"
#include "NPC.h"
#include "Globals.h"

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

void NPC::initPath(const distance_id_pair_type& nearest)
{
	if(!nearest.empty())
		mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID, mNearestTargets.begin()->second, Heuristic(Graph::Instance().getNode(mNearestTargets.begin()->second)));
}

void NPC::findNewPath()
{
	setPath();
	mNbTurnBlocked = 0;
}

//***TODO : HUGE MEMORY LEAK
std::list<unsigned int> NPC::pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic& h)
{

	Node* start{ graph.getNode(startID) };
	Node* goal{ graph.getNode(goalID) };

	//Init record
	NodeRecord* currentRecord = new NodeRecord{ start, 0, h.estimate(start, graph.getLevelInfo()) };

	//Init lists
	std::list<NodeRecord*> openedList{}, closedList{};
	openedList.emplace_back(currentRecord);

	NodeRecord* prevRecord;
	NodeRecord* endNodeRecord;
	while (!openedList.empty())
	{
		//Get smallest element
		currentRecord = *std::min_element(std::begin(openedList), std::end(openedList));
		Node* currentNode{ currentRecord->mNode };

		//Found goal - yay!
		if (currentNode == goal)
			break;

		//If not, check neighbours to find smallest cost step
		for (auto& neighbour : currentNode->getNeighbours())
		{
			if (!neighbour || neighbour->isNotAvailable())
			{
				continue;
			}

			//Node* endNode = neighbour;
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

	delete currentRecord;
	delete endNodeRecord;

	return finalPath;
}

void NPC::explore(std::vector<Action*>& actionList)
{
	auto currNode = Graph::Instance().getNode(mInfos.tileID);
	auto neighbourTiles = currNode->getNeighbours();
	int tileIdToExplore{};

	std::vector<Node*> accessibleNeighbours = getAccessibleNeighbours(neighbourTiles, currNode);
	tileIdToExplore = getLessVisitedTileId(accessibleNeighbours);
	mExploredTiles[tileIdToExplore]++;
	
	EDirection dir = chooseDirection(tileIdToExplore, mInfos.tileID);
	actionList.emplace_back(new Move{ mInfos.npcID, dir });
}

std::vector<Node*> NPC::getAccessibleNeighbours(const std::vector<Node*>& neighbourTiles, Node* currNode)
{
	std::vector<Node*> accessibleNeighbours;
	for (int i{}; i < neighbourTiles.size(); ++i)
	{
		if (neighbourTiles[i] && !neighbourTiles[i]->isNotAvailable()
			&& !currNode->hasWall(i))
		{
			accessibleNeighbours.emplace_back(neighbourTiles[i]);
		}
	}

	return accessibleNeighbours;
}

void NPC::followPath(std::vector<Action *> &actionList)
{
	EDirection dir = chooseDirection(mPathToGoal.front(), mInfos.tileID);
	actionList.emplace_back(new Move{ mInfos.npcID, dir });

	updatePathToGoal();
}

void NPC::setPath()
{
	if(hasGoal())
		mPathToGoal = pathFinderAStar(Graph::Instance(), mInfos.tileID, mGoalID, Heuristic(Graph::Instance().getNode(mGoalID)));
}

void NPC::update(std::vector<Action*>& actionList)
{
	if (hasGoal())
	{
		if (isArrived())
		{
			return;
		}
	
		if (isBlocked())
		{
			++mNbTurnBlocked;
			if (mNbTurnBlocked == 1)
			{
				return;
			}
			else if (mNbTurnBlocked == 2)
			{
				findNewPath();
			}
		}
	
		followPath(actionList);
	}
	else
	{
		explore(actionList);
	}
}

void NPC::updateInfo(const NPCInfo& npcInfo)
{
	mInfos = npcInfo;
}

unsigned int NPC::getTileID()
{
	return mInfos.tileID;
}