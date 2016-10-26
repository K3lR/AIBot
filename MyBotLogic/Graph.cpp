#include "Graph.h"
#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"
#include "TurnInfo.h"

#include <algorithm>
#include <iterator>
#include <queue>


Graph Graph::singletonGraph;

Graph::Graph() noexcept : mGraph{}, mTargets{}
{}

void Graph::addNode(Node* node)
{
	mGraph[node->getID()] = node;
}

void Graph::init(LevelInfo& lvlInfo)
{
	mLevelInfo = lvlInfo;

#pragma region Omniscient
	// mGraph.reserve(mLevelInfo.rowCount * mLevelInfo.colCount);

   //  addNode(new Node{ mLevelInfo.tiles[0] });

   //  //Horizontal node connections
   //  int idNeighbour = 0;
   //  std::for_each(std::next(std::begin(mLevelInfo.tiles)),
   //      std::end(mLevelInfo.tiles),
   //      [&idNeighbour, this](std::pair<unsigned int, TileInfo> currIterTile)
   //  {
   //      //Adds & connects both current & previous nodes
   //      if (currIterTile.second.tileID % mLevelInfo.colCount != 0)
   //          addNode(new Node{ currIterTile.second, mGraph[idNeighbour] });
   //      //Only adds
   //      else
   //          addNode(new Node{ currIterTile.second });

   //      if (std::find(
   //                      std::begin(currIterTile.second.tileAttributes),
   //                      std::end(currIterTile.second.tileAttributes),
   //                      TileAttribute_Target)
   //          != std::end(currIterTile.second.tileAttributes)
   //          )
   //      {
   //          mTargets.emplace_back(mGraph[currIterTile.second.tileID]);
   //      }

		 //mGraph[currIterTile.second.tileID]->setFlags(currIterTile.second.tileAttributes);


   //      ++idNeighbour;
   //  });

   //  //Vertical node connections
   //  connectSurroundings();

	 //for (auto& obj : mLevelInfo.objects)
	 //{
	 //	mGraph[obj.second.tileID]->setWalls(obj.second.edgesCost);
	 //}
	 //for (auto& npc : mLevelInfo.npcs)
	 //{
	 //	mGraph[npc.second.tileID]->taken() = true;
	 //}
#pragma endregion Omniscient


#pragma region Vision
	int nbTiles{ lvlInfo.colCount * lvlInfo.rowCount };

	addNode(new Node{ 0 });

	for (int i{ 1 }; i < nbTiles; ++i)
	{
		if (i % lvlInfo.colCount != 0)
			addNode(new Node{ i, mGraph.at(i - 1) });
		else
			addNode(new Node{ i });
	}
	connectSurroundings();

	initNodeFlags();
#pragma endregion Vision

}

void Graph::initNodeFlags()
{
	for (auto& npc : mLevelInfo.npcs)
	{
		mGraph[npc.second.tileID]->taken() = true;
	}
}

void Graph::connectSurroundings()
{
#pragma region Omniscient
	//int idNeighbour = 0;
	//std::for_each(mLevelInfo.tiles.find(mLevelInfo.colCount),
	//	mLevelInfo.tiles.end(),
	//	[&idNeighbour, this](std::pair<unsigned int, TileInfo> currIterTile)
	//{
	//	connectionEvenLinesOnRight(currIterTile, idNeighbour);
	//	++idNeighbour;
	//});
#pragma endregion Omniscient

#pragma region Vision
	int idNeighbour{ 0 };
	std::for_each(mGraph.find(mLevelInfo.colCount),
		mGraph.end(),
		[&idNeighbour, this](std::pair<unsigned int, Node*> currPairNode)
	{
		connectionEvenLinesOnRight(currPairNode.second->getID(), idNeighbour);
		++idNeighbour;
	});
#pragma endregion Vision
}

/* Links all nodes of a line with their corresponding above & under neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void Graph::connectionEvenLinesOnRight(const unsigned int& idNode, const int& idNeighbour)
{
	if ((idNode / mLevelInfo.colCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
	{
		if (idNode % mLevelInfo.colCount == mLevelInfo.colCount - 1)  //Dernière colonne (OK)
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
		else
		{
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
		}
	}
	else
		if (idNode % mLevelInfo.colCount == 0)
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
		else
		{
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
		}
}
//void Graph::connectionEvenLinesOnRight(const std::pair<unsigned int, TileInfo>& currIterTile, int idNeighbour)
//{
//	if ((currIterTile.second.tileID / mLevelInfo.colCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
//	{
//		if (currIterTile.second.tileID % mLevelInfo.colCount == mLevelInfo.colCount - 1)  //Dernière colonne (OK)
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
//		else
//		{
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
//		}
//	}
//	else
//		if (currIterTile.second.tileID % mLevelInfo.colCount == 0)
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
//		else
//		{
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
//			mGraph[currIterTile.second.tileID]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
//		}
//}

void Graph::updateMapInfo(TurnInfo& turnInfo)
{
	for (auto& tile : turnInfo.tiles)
	{
		mGraph[tile.first]->updateInfo(tile.second);
		if (std::find(
			std::begin(tile.second.tileAttributes),
			std::end(tile.second.tileAttributes),
			TileAttribute_Target)
			!= std::end(tile.second.tileAttributes)
			)
		{
			mTargets.emplace_back(mGraph[tile.first]);
		}
	}
	for (auto& obj : turnInfo.objects)
	{
		mGraph[obj.second.tileID]->setWalls(obj.second.edgesCost);
	}
}

