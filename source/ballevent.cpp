#include <boss.hpp>
#include "ballevent.hpp"

BallEvent& BallEvent::operator=(const BallEvent& rhs)
{
    mWrittenMsec = rhs.mWrittenMsec;
    mStatus = rhs.mStatus;
    return *this;
}

float BallEvent::DistanceTo(float x, float y) const
{
    return Math::Distance(mStatus.mPosX, mStatus.mPosY, x, y);
}
