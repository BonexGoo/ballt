#pragma once
#include <ballevent.hpp>

class WaveComp
{
protected:
    WaveComp();
    virtual ~WaveComp();

protected:
    static const sint32 mBallR {2}; // 전파영향권 반지름(20cm)
    static const sint32 mCarR {25}; // 차량 반지름(2.5m)
    static const sint32 mWaveR {50}; // 전파영향권 반지름(5m)
    static const sint32 mRelayDist {1000}; // 릴레이 한계거리(100m)
    static const uint64 mSlowVideo {100}; // 슬로우비디오계수(100x)

protected:
    void MoveToCell(sint32 x, sint32 y);
    void WaveFlush(const BallEvents& events, float waveR);
    virtual void OnWaveSniff(const BallEvents& events, float waveR) = 0;

private:
    sint32 mCellX;
    sint32 mCellY;
};
