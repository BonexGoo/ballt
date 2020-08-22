#pragma once
#include <service/boss_zay.hpp>
#include <railball.hpp>
#include <car.hpp>

class balltData : public ZayObject
{
public:
    balltData();
    ~balltData();

public:
    void RenderInfo(ZayPanel& panel);

public:
    Map<RailBall> mBalls;
    Map<Car> mCars;

public:
    bool mKeyLeft;
    bool mKeyUp;
    bool mKeyRight;
    bool mKeyDown;
};
