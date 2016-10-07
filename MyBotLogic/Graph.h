#ifndef GRAPH_H
#define GRAPH_H

#include "Incopiable.h"
#include <algorithm>
#include <vector>
#include <list>

class Heuristic;
class Node;

struct LevelInfo;
struct TileInfo;
struct TurnInfo;

class Graph : Incopiable
{
public: 
    using cost_type = unsigned int;

private:
    static Graph singletonGraph;

    int mRowCount, mColCount, mInvColCount; //TODO : relocate in MyBotLogic
    std::vector<TileInfo*> mTargets;        //TODO : relocate in MyBotLogic
    
    std::vector<Node*> mGraph;

    Graph() noexcept;
    void addNode(Node* node) { mGraph.emplace_back(node); }
    void connectSurroundings(TurnInfo&);
    void connectionEvenLinesOnRight(const std::pair<unsigned int, TileInfo>&, int);

    //***TODO : relocate in MyBotLogic
    //bool isForbidden(Node* node);

public : 
    //***TODO : relocate in MyBotLogic
    //const cost_type CONNECTION_COST{ 10 };

    ~Graph() {}
    static Graph& Instance() noexcept { return singletonGraph; }

    void init(LevelInfo&);
    void createGraph(TurnInfo& turnInfo);

    bool isEmpty() const { return mGraph.empty(); }

    Node* getNode(int i) const { return mGraph[i]; }

    //***TODO : relocate in MyBotLogic
    /*std::list<unsigned int> pathFinderAStar(const unsigned int& startID, const unsigned int& goalID, Heuristic&);*/

};

#endif // !GRAPH_H

#ifndef NODE_RECORD_H
#define NODE_RECORD_H

struct NodeRecord
{
    //enum State { UNVISITED, OPEN, CLOSED };

    using cost_type = Graph::cost_type;
    //using category_type = State;

    Node* mNode;
    NodeRecord* mPrevious;
    cost_type mCostSoFar;
    cost_type mEstimatedTotalCost;
    //category_type mState;
    //NodeRecord* mNextRecord;
    
    NodeRecord()
        : mNode{ nullptr }, mPrevious{ nullptr }, mCostSoFar{ 0 }, mEstimatedTotalCost{ 0 }/*, mState{ UNVISITED }, mNextRecord{ nullptr }*/
    {}
    NodeRecord(NodeRecord* other)
        : mNode{ other->mNode }, mPrevious{ other->mPrevious }, mCostSoFar{ other->mCostSoFar }, mEstimatedTotalCost{ other->mEstimatedTotalCost }
    {}
    NodeRecord(Node* n, const cost_type& costSoFar, const cost_type& estimatedCost)
        : mNode{ n }, mPrevious{ nullptr }, mCostSoFar{ costSoFar }, mEstimatedTotalCost{ estimatedCost }/*, mState{ UNVISITED }, mNextRecord{}*/
    {}

    bool operator<(NodeRecord* other)
    {
        return mEstimatedTotalCost < other->mEstimatedTotalCost;
    }
    bool operator==(NodeRecord* other)
    {
        return mNode == other->mNode;
    }

    bool empty() { return !mNode; }

    static NodeRecord* findIn(std::list<NodeRecord*> nrList, Node* n)
    {
        for (auto nr : nrList)
        {
            if (nr->mNode == n)
                return nr;
        }

        return nullptr;
    }
};

#endif // !NODE_RECORD_H