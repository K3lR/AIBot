#ifndef NPC_H
#define NPC_H

#include <list>

struct NPCInfo;

struct NPC
{
    using cost_type = MyBotLogic::cost_type;
    using distance_id_pair_type = MyBotLogic::distance_id_pair_type;

private:
    enum EState{START, BLOCKED, MOVING, WAITING, ARRIVED};

public:
    NPC(const NPCInfo& infos, const distance_id_pair_type& nearest, const std::list<unsigned int>& path)
        : mInfos{ infos }, mNearestTargets{ nearest }, mPathToGoal{ path },
        mNextTileID{ infos.tileID }/*, mState{ START }, mTurnWait{ 0 }*/
    {}
    ~NPC() {}

    NPCInfo mInfos;
    unsigned int mNextTileID;
    EState mState;

    distance_id_pair_type mNearestTargets;  //Sorted targets ID from nearest to furthest
    std::list<unsigned int> mPathToGoal;    //Path of Tile IDs to goal

//    bool isArrived() const { return mState == ARRIVED; }
//    bool isBlocked() const { return mState == BLOCKED; }
//    bool isMoving() const { return mState == MOVING; }
//    bool isWaiting() const { return mState == WAITING; }
//    
//    bool operator==(const NPC& other)
//    {
//        return mInfos.tileID == other.mInfos.tileID;
//    }
//
//    void checkObstacles(const std::vector<NPC> npcs)
//    {
//        for (int i{}; i < npcs.size(); ++i)
//        {
//            if (i == mInfos.npcID - 1)
//                continue;
//
//            if (mPathToGoal.front() == npcs[i].mNextTileID)
//                setState(WAITING);
//        }
//    }
//    void updateState(const std::vector<NPC>& npcs)
//    {
//        setState(MOVING);
//
//        if (mPathToGoal.empty())
//            setState(ARRIVED);
//
//        checkObstacles(npcs);
//
//        if (mTurnWait >= 2)
//            setState(BLOCKED);
//
//        update();
//    }
//
//private:
//    unsigned int mTurnWait;
//    void setState(const EState& nxtState) { mState = nxtState; }
//    void update()
//    {
//        if (isMoving())
//        {
//            mNextTileID = mPathToGoal.front();
//            mPathToGoal.pop_front();
//            mTurnWait = 0;
//        }
//        else if (isWaiting())
//        {
//            ++mTurnWait;
//        }
//        else if (isBlocked())
//        {
//            mNearestTargets.erase(mNearestTargets.begin());
//            mTurnWait = 0;
//        }
//    }

};

#endif //!NPC8_H
