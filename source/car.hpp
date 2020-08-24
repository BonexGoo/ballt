#pragma once
#include <service/boss_zay.hpp>
#include <wavecomp.hpp>

class Car : public WaveComp
{
    BOSS_DECLARE_NONCOPYABLE_CLASS(Car)

public:
    Car();
    ~Car() override;

public:
    static sint32 FocusedCar();

public:
    void Init(double x, double y);
    void RunOnce();
    void BreakOnce();
    void RotateOnce(bool left);

public:
    void RenderCar(ZayPanel& panel);
    void RenderCarBG(ZayPanel& panel);
    void Tick(uint64 msec);

private:
    void OnWaveSniff(const BallEvents& events, float waveR) override;

private:
    sint32 mCarID {0};
    double mPosX {0};
    double mPosY {0};
    double mPosZ {0};
    float mCarAngle {0};
    float mTireAngle {0};
    float mSpeed {0};

private: // 자율주행
    uint32 mTargetCode {0};
    float mTargetOrder {0};
    BallEvents mTargetEvents;

private: // 랜더링 요소
    String mUIName;
    sint32 mCarAni;
};
