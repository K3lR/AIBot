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
	mLevelInfo = lvlInfo;
    mGraph.reserve(mLevelInfo.rowCount * mLevelInfo.colCount);

    addNode(new Node{ mLevelInfo.tiles[0] });

    //Horizontal node connections
    int idNeighbour = 0;
    std::for_each(std::next(std::begin(mLevelInfo.tiles)),
        std::end(mLevelInfo.tiles),
        [&idNeighbour, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        //Adds & connects both current & previous nodes
        if (currIterTile.second.tileID % mLevelInfo.colCount != 0)
            addNode(new Node{ currIterTile.second, mGraph[idNeighbour] });
        //Only adds
        else
            addNode(new Node{ currIterTile.second });

        if (std::find(
                        std::begin(currIterTile.second.tileAttributes),
                        std::end(currIterTile.second.tileAttributes),
                        TileAttribute_Target)
            != std::end(currIterTile.second.tileAttributes)
            )
        {
            mTargets.emplace_back(mGraph[currIterTile.second.tileID]);
        }

        ++idNeighbour;
    });

    //Vertical node connections
    connectSurroundings();
}

void Graph::connectSurroundings()
{
    int idNeighbour = 0;
    std::for_each(mLevelInfo.tiles.find(mLevelInfo.colCount),
		mLevelInfo.tiles.end(),
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
    if ((currIterTile.second.tileID / mLevelInfo.colCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
    {
        if (currIterTile.second.tileID % mLevelInfo.colCount == mLevelInfo.colCount - 1)  //Dernière colonne (OK)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
        }
    }
    else
        if (currIterTile.second.tileID % mLevelInfo.colCount == 0)
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
        }
}
