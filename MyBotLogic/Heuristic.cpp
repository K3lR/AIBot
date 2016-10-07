#include "Heuristic.h"
#include "Node.h"


Heuristic::cost_type Heuristic::estimate(Node * start)
{
    return manhattanDistance(start, mGoalNode);
}

Heuristic::cost_type Heuristic::manhattanDistance(Node* start, Node* goal)
{
    return 0;
    /*return subUInt(start->getID() % mColCount, goal->getID() % mColCount)
    + subUInt(start->getID() * mInvColCount, goal->getID() * mInvColCount);*/
}