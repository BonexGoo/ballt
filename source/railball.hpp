#pragma once
#include <service/boss_zay.hpp>

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
    BallStatus& operator=(const BallStatus& rhs)
    {Memory::Copy(&mRailCode, &rhs.mRailCode, sizeof(BallStatus) - sizeof(uint32)); return *this;}
};

class BallEvent
{
public:
    uint64 mWaveMsec {0};
    BallStatus mStatus;

public:
    BallEvent& operator=(const BallEvent& rhs)
    {
        mWaveMsec = rhs.mWaveMsec;
        mStatus = rhs.mStatus;
        return *this;
    }
};

typedef Array<BallEvent> BallEvents;

class RailBall : public BallEvent
{
    BOSS_DECLARE_NONCOPYABLE_CLASS(RailBall)

public:
    RailBall();
    ~RailBall();

public:
    static void RenderRailCode(ZayPanel& panel);

public:
    void Init(double x, double y);
    void MoveToCell(sint32 x, sint32 y);

public:
    void RenderBall(ZayPanel& panel);
    void RenderInfo(ZayPanel& panel);
    void RenderWave(ZayPanel& panel);
    void Tick(uint64 msec);

private:
    void RememberMe();
    void WaveFlush(const BallEvents& events, float waveR);

private: // 상수
    static const sint32 mWaveR {50}; // 전파영향권 반지름(5M)
    static const sint32 mRelayDist {1000}; // 릴레이 한계거리(100M)
    static const uint64 mSlowVideo {100}; // 슬로우비디오계수(100배)

private: // 수집이벤트
    Map<BallEvent> mTotalEvents;

private: // 이벤트 처리과정
    BallEvents mNextEvents;
    BallEvents mLiveEvents;
    sint32 mCellX;
    sint32 mCellY;
};
