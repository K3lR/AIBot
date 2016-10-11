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

    std::vector<TileInfo*> mTargets;        //TODO : relocate in MyBotLogic
    
    std::vector<Node*> mGraph;

    Graph() noexcept;
    void addNode(Node* node) { mGraph.emplace_back(node); }
    void connectSurroundings(TurnInfo&, LevelInfo&);
    void connectionEvenLinesOnRight(LevelInfo&, const std::pair<unsigned int, TileInfo>&, int);
    void init(const LevelInfo&);

public : 
    ~Graph() {}
    static Graph& Instance() noexcept { return singletonGraph; }
    
    void createGraph(TurnInfo&, LevelInfo&);
    std::vector<Node*> getGraph() const noexcept { return mGraph; }
    Node* getNode(int i) const { return mGraph[i]; }
    bool isEmpty() const noexcept { return mGraph.empty(); }
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