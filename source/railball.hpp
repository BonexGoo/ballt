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
    uint64 mWrittenMsec {0};
    BallStatus mStatus;

public:
    BallEvent& operator=(const BallEvent& rhs)
    {
        mWrittenMsec = rhs.mWrittenMsec;
        mStatus = rhs.mStatus;
        return *this;
    }

public:
    float DistanceTo(const BallEvent& other)
    {
        return Math::Distance(mStatus.mPosX, mStatus.mPosY,
            other.mStatus.mPosX, other.mStatus.mPosY);
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
    void RenderWaveBG(ZayPanel& panel);
    void Tick(uint64 msec);

private:
    void RememberMe();
    void WaveFlush(const BallEvents& events, float waveR);

private: // 상수
    static const sint32 mBallR {2}; // 전파영향권 반지름(20cm)
    static const sint32 mCarR {25}; // 차량 반지름(2.5m)
    static const sint32 mWaveR {50}; // 전파영향권 반지름(5m)
    static const sint32 mRelayDist {1000}; // 릴레이 한계거리(100m)
    static const uint64 mSlowVideo {100}; // 슬로우비디오계수(100x)

private: // 수집이벤트
    Map<BallEvent> mTotalEvents;

private: // 이벤트 처리과정
    BallEvents mNextEvents;
    BallEvents mLiveEvents;
    uint64 mWaveMsec;
    sint32 mCellX;
    sint32 mCellY;

private: // 랜더링 요소
    String mUIName;
    float mBallAni;
    float mBallSizeR;
};
