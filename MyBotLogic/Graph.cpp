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
void Graph::init(const LevelInfo& lvlInfo)
{
    mGraph.reserve(lvlInfo.rowCount * lvlInfo.colCount);
}

void Graph::createGraph(TurnInfo& turnInfo, LevelInfo& lvlInfo)
{
    addNode(new Node{ turnInfo.tiles[0] });

    //Horizontal node connections
    int idNeighbour = 0;
    std::for_each(std::next(std::begin(turnInfo.tiles)),
        std::end(turnInfo.tiles),
        [&idNeighbour, &lvlInfo, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        //Adds & connects both current & previous nodes
        if (currIterTile.second.tileID % lvlInfo.colCount != 0)
            addNode(new Node{ currIterTile.second, mGraph[idNeighbour] });
        //Only adds
        else
            addNode(new Node{ currIterTile.second });

        ++idNeighbour;
    });

    //Vertical node connections
    connectSurroundings(turnInfo, lvlInfo);
}

void Graph::connectSurroundings(TurnInfo& turnInfo, LevelInfo& lvlInfo)
{
    int idNeighbour = 0;
    std::for_each(turnInfo.tiles.find(lvlInfo.colCount),
        turnInfo.tiles.end(),
        [&idNeighbour, &lvlInfo, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        connectionEvenLinesOnRight(lvlInfo, currIterTile, idNeighbour);
        ++idNeighbour;
    });
}

/* Links all nodes of a line with their corresponding above & under neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void Graph::connectionEvenLinesOnRight(LevelInfo& lvlInfo, const std::pair<unsigned int, TileInfo>& currIterTile, int idNeighbour)
{
    if ((currIterTile.second.tileID / lvlInfo.colCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
    {
        if (currIterTile.second.tileID % lvlInfo.colCount == lvlInfo.colCount - 1)  //Dernière colonne (OK)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
        }
    }
    else
        if (currIterTile.second.tileID % lvlInfo.colCount == 0)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        }
}
