#include <boss.hpp>
#include "ballstatus.hpp"

BallStatus& BallStatus::operator=(const BallStatus& rhs)
{
    Memory::Copy(&mRailCode, &rhs.mRailCode, sizeof(BallStatus) - sizeof(uint32));
    return *this;
}
