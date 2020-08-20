#pragma once
#include <service/boss_zay.hpp>
#include <railball.hpp>

class balltData : public ZayObject
{
public:
    balltData();
    ~balltData();

public:
    Map<RailBall> mBalls;
};
