#ifndef NODE_H
#define NODE_H

#include "TileInfo.h"
#include <vector>


class Node 
{
public:
    enum { NB_NEIGHBOURS = 6 };

private:
    TileInfo mTileInfo;

    Node* mFrom;                        //Maybe not so useful...
    std::vector<Node*> mNeighbours;     //CW - 0 on right side
	bool mWalls[NB_NEIGHBOURS] {};
	struct Flags
	{
		//***TODO : flag taken, changer obstacle => hasWall, hasWindow
		bool taken;
		bool obstacle;
		bool forbidden;
	} mFlags{};

public:
    Node(Node* other)
        : mTileInfo(other->mTileInfo), mFrom{ other->mFrom }, mNeighbours{ other->mNeighbours }
    {}
    Node(TileInfo& currTileInfo)
        : mTileInfo( currTileInfo ), mFrom{ nullptr }, mNeighbours{ NB_NEIGHBOURS }
    {}
    Node(TileInfo& currTileInfo, Node* from)
        : mTileInfo(currTileInfo), mFrom{ from }, mNeighbours{ NB_NEIGHBOURS }
    {
        mNeighbours[0] = nullptr;
        mNeighbours[1] = nullptr;
        mNeighbours[2] = nullptr;
        mNeighbours[3] = from;
        mNeighbours[4] = nullptr;
        mNeighbours[5] = nullptr;

        from->mNeighbours[0] = this;
    }

    ~Node()
    {
        delete mFrom;
    }

    unsigned int getID() const { return mTileInfo.tileID; }
    std::set<ETileAttribute>& getTileAttributes() { return mTileInfo.tileAttributes; }
    std::vector<Node*> getNeighbours() const { return mNeighbours; }
    Node* getNeighbour(int i) const { return mNeighbours[i]; }
    void setNeighbour(Node* neighbour, unsigned int idxNeighb, unsigned int idxCurr)
    {
        mNeighbours[idxNeighb] = neighbour;
        neighbour->mNeighbours[idxCurr] = this;
    }
	void setForbiddenFlag(const bool& flagVal) { mFlags.forbidden = flagVal; }
	void setFlags(const std::set<ETileAttribute>& tileAttributes)
	{
		if (std::find(
			std::begin(tileAttributes),
			std::end(tileAttributes),
			TileAttribute_Obstacle)
			!= std::end(tileAttributes)
			)
		{
			mFlags.obstacle = true;
		}
		if (std::find(
			std::begin(tileAttributes),
			std::end(tileAttributes),
			TileAttribute_Forbidden)
			!= std::end(tileAttributes)
			)
		{
			mFlags.forbidden = true;
		}
	}
	void setWalls(unsigned int* edgecost)
	{
		unsigned int id{};
		for (unsigned int* i = edgecost; i < edgecost + 8; ++i, ++id)
		{
			unsigned int eCost = *i;
			if (!*i)
			{
				unsigned int idNeighbour{};
				if (id <= 1)
				{
					idNeighbour = 5;
				}
				else if (id <= 3)
				{
					idNeighbour = id - 2;
				}
				else if (id <= 7)
				{
					idNeighbour = id - 3;
				}
				mWalls[idNeighbour] = true;
			}
		}
	}
	bool isNotAvailable() const
	{
		return mFlags.obstacle || mFlags.forbidden;
	}
	bool hasWall(const unsigned int& idNeighbour) const
	{
		return mWalls[idNeighbour];
	}
	bool& taken() { return mFlags.taken; }
	bool& obstacle() { return mFlags.obstacle; }
};

#endif // !NODE_H
