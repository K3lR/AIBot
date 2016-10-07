#include "Graph.h"
#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"
#include "TurnInfo.h"

#include <algorithm>
#include <iterator>
#include <queue>

//Debug
#include <cassert>

Graph Graph::singletonGraph;

Graph::Graph() noexcept : mGraph{}, mTargets{}
{}
void Graph::init(LevelInfo& lvlInfo)
{
    //****TODO : relocate in MyBotLogic
    mRowCount = lvlInfo.rowCount;
    mColCount = lvlInfo.colCount;
    mInvColCount = 1 / mColCount;


    mGraph.reserve(mRowCount * mColCount);
}

void Graph::createGraph(TurnInfo& turnInfo)
{
    addNode(new Node{ turnInfo.tiles[0] });

    //Horizontal node connections
    int idNeighbour = 0;
    std::for_each(std::next(std::begin(turnInfo.tiles)),
        std::end(turnInfo.tiles),
        [&idNeighbour, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        //Adds & connects both current & previous nodes
        if (currIterTile.second.tileID % mColCount != 0)
            addNode(new Node{ currIterTile.second, mGraph[idNeighbour] });
        //Only adds
        else
            addNode(new Node{ currIterTile.second });

        //***TODO : relocate in NPC's class
        //Looking for targets / goals
        if (std::find(std::begin(currIterTile.second.tileAttributes),
            std::end(currIterTile.second.tileAttributes),
            TileAttribute_Target)
            != std::end(currIterTile.second.tileAttributes)
            )
        {
            mTargets.emplace_back(new TileInfo{ currIterTile.second });
        }

        ++idNeighbour;
    });

    //Vertical node connections
    connectSurroundings(turnInfo);
}

void Graph::connectSurroundings(TurnInfo& turnInfo)
{
    int idNeighbour = 0;
    std::for_each(turnInfo.tiles.find(mColCount),
        turnInfo.tiles.end(),
        [&idNeighbour, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        connectionEvenLinesOnRight(currIterTile, idNeighbour);
        ++idNeighbour;
    });
}

/* Links all nodes of a line with their corresponding above & under neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void Graph::connectionEvenLinesOnRight(const std::pair<unsigned int, TileInfo>& currIterTile, int idNeighbour)
{
    if ((currIterTile.second.tileID / mColCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
    {
        if (currIterTile.second.tileID % mColCount == mColCount - 1)  //Dernière colonne (OK)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
        }
    }
    else
        if (currIterTile.second.tileID % mColCount == 0)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        }
}

//***TODO : relocate in MyBotLogic
//bool Graph::isForbidden(Node* node)
//{
//    if (!node)
//        return false;
//
//    return std::find(node->getTileAttributes().begin(),
//        node->getTileAttributes().end(),
//        TileAttribute_Forbidden)
//        != node->getTileAttributes().end();
//}

//***TODO : relocate in MyBotLogic
//std::list<unsigned int> Graph::pathFinderAStar(const unsigned int& startID, const unsigned int& goalID, Heuristic& h)
//{
//
//    Node* start{ mGraph[startID] };
//    Node* goal{ mGraph[goalID] };
//
//    //Init record
//    NodeRecord* currentRecord = new NodeRecord{ start, 0, h.estimate(start) };
//
//    //Init lists
//    std::list<NodeRecord*> openedList{}, closedList{};
//    openedList.emplace_back(currentRecord);
//
//    while (!openedList.empty())
//    {
//
//        //Get smallest element
//        currentRecord = *std::min_element(std::begin(openedList), std::end(openedList));
//        NodeRecord* prevRecord = new NodeRecord{};
//        Node* currentNode{ currentRecord->mNode };
//
//        //Found goal - yay!
//        if (currentNode == goal)
//            break;
//
//        //If not, check neighbours to find smallest cost step
//        NodeRecord* endNodeRecord = new NodeRecord{};
//        for(auto& neighbour : currentNode->getNeighbours())
//        {
//            if (!neighbour || isForbidden(neighbour))
//                continue;
//
//            //Node* endNode = neighbour;
//            cost_type endNodeCost = currentRecord->mCostSoFar + CONNECTION_COST;
//            cost_type endNodeHeuristic{ 0 };
//
//            //If closed node, may have to skip or remove it from closed list
//            //**Remark : nRecord <-> endNodeRecord
//            if ((endNodeRecord = NodeRecord::findIn(closedList, neighbour)))
//            {
//                //If not a shorter route, skip
//                if (endNodeRecord->mCostSoFar <= endNodeCost)
//                    continue;
//
//                //If shorter, we need to put it back in the opened list
//                closedList.remove(endNodeRecord);
//
//                //Update heuristic
//                endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
//            }
//            else if ((endNodeRecord = NodeRecord::findIn(openedList, neighbour)))
//            {
//                if (endNodeRecord->mCostSoFar <= endNodeCost)
//                    continue;
//
//                endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
//            }
//            //Unvited node : need a new record
//            else
//            {
//                endNodeRecord = new NodeRecord{ neighbour, 0, h.estimate(neighbour) };
//            }
//
//            //Update record
//            endNodeRecord->mCostSoFar = endNodeCost;
//            endNodeRecord->mPrevious = currentRecord;
//            endNodeRecord->mEstimatedTotalCost = endNodeCost + endNodeHeuristic;
//
//            //Put record back in opened list
//            openedList.emplace_back(endNodeRecord);
//
//        }
//        //Update previous
//        prevRecord = currentRecord;
//
//        //Looked at all current node's neighbours : remove opened, put it in closed
//        openedList.remove(currentRecord);
//        closedList.emplace_back(currentRecord);
//    }
//
//    //Found our goal or run out of nodes
//    std::list<unsigned int> finalPath{};
//    if (currentRecord->mNode != goal)
//        return finalPath;
//
//    else
//    {
//        //Work back along path
//        while (currentRecord->mNode != start)
//        {
//            finalPath.emplace_front(currentRecord->mNode->getID());
//            currentRecord = currentRecord->mPrevious;
//        }
//    }
//
//    /*************************************************************************************************
//    struct NodeRecord
//    {
//        Node* mNode;
//        Node* mNeighbour;
//        cost_type mCostSoFar;
//        cost_type mEstimatedTotalCost;
//
//        NodeRecord() {}
//
//        NodeRecord(Node* start)
//            : mNode{ nullptr } {}
//        NodeRecord(Node* node, Node* neighb, const cost_type& cost, cost_type estimate)
//            : mNode{ node }, mNeighbour{ neighb }, mCostSoFar{ cost }, mEstimatedTotalCost{ estimate }
//        {}
//
//        static NodeRecord& findNode(std::list<NodeRecord>& list, Node* node)
//        {
//            NodeRecord nrecord;
//            for_each(std::begin(list),
//                std::end(list),
//                [&](NodeRecord& nr)
//                {
//
//                    if (nr.mNode == node)
//                    {
//                        nrecord = nr;
//                    }
//                });
//
//            return nrecord;
//        }
//        bool operator<(const NodeRecord& other)
//        {
//            return mCostSoFar < other.mCostSoFar;
//        }
//
//        bool isEmpty() { return !mNode; }
//
//    };
//
//    //Initialize start node record
//    NodeRecord startNodeRecord{ start, nullptr, 0, h.estimate(start) };
//
//    //Init lists
//    std::list<NodeRecord> openedNodes{}, closedNodes{};
//    openedNodes.emplace_back(startNodeRecord);
//
//    NodeRecord currNR{};
//    while (!openedNodes.empty())
//    {
//        cost_type endNodeHeuristic{ 0 };
//
//        currNR = *(std::min_element(openedNodes.begin(), openedNodes.end()));
//
//        if (currNR.mNode == goal)
//            break;
//
//        for (int i{}; i < currNR.mNode->getNeighbours().size() && currNR.mNode->getNeighbours()[i]; ++i)
//        {
//            Node* neighbour{ currNR.mNode->getNeighbours()[i] };
//            cost_type endNodeCost{ currNR.mCostSoFar + CONNECTION_COST };
//            NodeRecord endNodeRecord{ NodeRecord::findNode(closedNodes, neighbour) };
//
//            //Find if our current neighbour / connection is in our closed list
//            if (!endNodeRecord.isEmpty())
//            {
//                if (endNodeRecord.mCostSoFar <= endNodeCost)
//                    continue;
//
//                closedNodes.remove(endNodeRecord);
//                endNodeHeuristic = endNodeRecord.mEstimatedTotalCost - endNodeRecord.mCostSoFar;
//            }
//            //Find if our current neighbour / connection is in our opened list
//            else if (!(endNodeRecord = NodeRecord::findNode(openedNodes, neighbour)).isEmpty())
//            {
//                if (endNodeRecord.mCostSoFar <= endNodeCost)
//                    continue;
//
//                endNodeHeuristic = endNodeRecord.mEstimatedTotalCost - endNodeRecord.mCostSoFar;
//            }
//            //Unvisited neighbour
//            else
//            {
//                //*************TODO : conflict with l.212
//                //endNodeRecord.mNode = neighbour;
//                endNodeHeuristic = h.estimate(endNodeRecord.mNode);
//            }
//
//            //We're here if we need to update the node (??)
//            endNodeRecord.mCostSoFar = endNodeCost;
//            endNodeRecord.mNeighbour = neighbour;
//            endNodeRecord.mEstimatedTotalCost = endNodeCost + endNodeHeuristic;
//
//            //Find if our current neighbour / connection is NOT in our opened list
//            if (NodeRecord::findNode(openedNodes, neighbour).isEmpty())
//            {
//                openedNodes.emplace_back(endNodeRecord);
//            }
//        }
//
//        //Done looking at current node's neighbours
//        openedNodes.remove(currNR);
//        closedNodes.emplace_back(currNR);
//    }
//
//    //Run out of nodes
//    std::list<Node*> finalPath{};
//    if (currNR.mNode != goal)
//    {
//        return finalPath;
//    }
//    //Found a path
//    else
//    {
//        Node* currNode = std::move(new Node{*currNR.mNode});
//        finalPath.emplace_front(currNode);
//        while (currNode != start)
//        {
//            finalPath.push_front(currNR.mNeighbour);
//            currNode = currNR.mNeighbour;
//        }
//    }
//
//    return finalPath;
//    */
//
//    return finalPath;
//}

