#pragma once
#include <service/boss_zay.hpp>
#include <wavecomp.hpp>

class RailBall : public WaveComp
{
    BOSS_DECLARE_NONCOPYABLE_CLASS(RailBall)

public:
    RailBall();
    ~RailBall() override;

public:
    static void RenderRailCode(ZayPanel& panel);

public:
    void Init(double x, double y);

public:
    void RenderBall(ZayPanel& panel);
    void RenderInfo(ZayPanel& panel);
    void RenderWave(ZayPanel& panel);
    void RenderWaveBG(ZayPanel& panel);
    void Tick(uint64 msec);

private:
    void RememberMe();
    void OnWaveSniff(const BallEvents& events, float waveR) override;

private:
    BallStatus mStatus;
    Map<BallEvent> mTotalEvents;

private: // 이벤트 처리과정
    BallEvents mNextEvents;
    BallEvents mLiveEvents;
    uint64 mWaveMsec;

private: // 랜더링 요소
    String mUIName;
    float mBallAni;
    float mBallSizeR;
};
