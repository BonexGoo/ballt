#pragma once
#include <ballstatus.hpp>

class BallEvent
{
public:
    uint64 mWrittenMsec {0};
    BallStatus mStatus;

public:
    BallEvent& operator=(const BallEvent& rhs);

public:
    float DistanceTo(float x, float y) const;
};
typedef Array<BallEvent> BallEvents;
