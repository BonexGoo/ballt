#pragma once
#include <boss.hpp>

class BallStatus
{
public:
    const uint32 mVersion {0x10000};
    uint32 mRailCode {0};
    float mRailOrder {0};
    uint32 mJuncCode {0}; // 연결가능한 RailCode
    double mPosX {0};
    double mPosY {0};
    double mPosZ {0};

public:
    BallStatus& operator=(const BallStatus& rhs);
};
