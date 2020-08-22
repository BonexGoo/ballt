#pragma once
#include <service/boss_zay.hpp>

class Car
{
public:
    Car();
    ~Car();

public:
    static sint32 FocusedCar();

public:
    void Init(double x, double y);
    void MoveToCell(sint32 x, sint32 y);
    void RunOnce();
    void BreakOnce();
    void RotateOnce(bool left);

public:
    void RenderCar(ZayPanel& panel);
    void RenderCarBG(ZayPanel& panel);
    void Tick(uint64 msec);

private: // 상수
    static const sint32 mBallR {2}; // 전파영향권 반지름(20cm)
    static const sint32 mCarR {25}; // 차량 반지름(2.5m)
    static const sint32 mWaveR {50}; // 전파영향권 반지름(5m)
    static const sint32 mRelayDist {1000}; // 릴레이 한계거리(100m)
    static const uint64 mSlowVideo {100}; // 슬로우비디오계수(100x)

private:
    sint32 mCarID {0};
    double mPosX {0};
    double mPosY {0};
    float mCarAngle {0};
    float mTireAngle {0};
    float mSpeed {0};

private:
    sint32 mCellX;
    sint32 mCellY;
};
