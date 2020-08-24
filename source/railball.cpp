#include <boss.hpp>
#include "railball.hpp"

#include <resource.hpp>

RailBall::RailBall()
{
    mWaveMsec = 0;
    mBallAni = 0;
    mBallSizeR = 0;
}

RailBall::~RailBall()
{
}

static uint32 gRailCode = 1;
static float gRailOrder = 1;
void RailBall::RenderRailCode(ZayPanel& panel)
{
    ZAY_INNER_UI(panel, 0, "next",
        ZAY_GESTURE_T(t)
        {
            if(t == GT_InReleased)
            {
                gRailCode++;
                gRailOrder = 1;
            }
        })
    ZAY_MOVE_IF(panel, 1, 1, panel.state("next") & (PS_Pressed | PS_Dragging))
    {
        ZAY_RGB(panel, 255, 255, 255)
        ZAY_RGB_IF(panel, 80, 80, 80, panel.state("next") & PS_Focused)
            panel.fill();
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
        ZAY_RGB(panel, 0, 0, 0)
        {
            auto Text = String::Format("NEXT: %u-%u", gRailCode, (uint32) gRailOrder);
            panel.text(Text);
        }
    }
}

void RailBall::Init(double x, double y)
{
    mStatus.mRailCode = gRailCode;
    mStatus.mRailOrder = gRailOrder;
    mStatus.mPosX = x;
    mStatus.mPosY = y;
    MoveToCell(((sint32) x) / mWaveR, ((sint32) y) / mWaveR);
    gRailOrder += 1;

    auto& NewEvent = mLiveEvents.AtAdding();
    NewEvent.mWrittenMsec = Platform::Utility::CurrentTimeMsec();
    NewEvent.mStatus = mStatus;
    mWaveMsec = Platform::Utility::CurrentTimeMsec();

    mUIName = String::Format("%u-%u",
        mStatus.mRailCode, (uint32) mStatus.mRailOrder);
    mBallAni = 0;
    mBallSizeR = 0;
}

void RailBall::RenderBall(ZayPanel& panel)
{
    // 랜더링 요소 재계산
    mBallAni = 0;
    const uint64 WaveMsec = Platform::Utility::CurrentTimeMsec() - mWaveMsec;
    if(WaveMsec < 2 * mSlowVideo) // 0.2초
        mBallAni = 1 - WaveMsec / (2.0 * mSlowVideo);

    const sint32 UIGap = 4;
    mBallSizeR = mBallR + UIGap
        * ((panel.state(mUIName) & (PS_Focused | PS_Dragging))? 1.0 : mBallAni);

    // 볼
    ZAY_XYRR_UI(panel, mStatus.mPosX, mStatus.mPosY, mBallR + UIGap, mBallR + UIGap, mUIName,
        ZAY_GESTURE_TXY(t, x, y, this)
        {
            static Point OldPos;
            if(t == GT_Pressed)
                OldPos = Point(x, y);
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                mStatus.mPosX += x - OldPos.x;
                mStatus.mPosY += y - OldPos.y;
                MoveToCell(((sint32) mStatus.mPosX) / mWaveR, ((sint32) mStatus.mPosY) / mWaveR);
                OldPos = Point(x, y);
            }
        })
    ZAY_XYRR(panel, panel.w() / 2, panel.h() / 2, mBallSizeR, mBallSizeR)
    {
        // 외형
        ZAY_INNER(panel, -1)
        ZAY_RGBA(panel, 255, 255, 255, 128)
            panel.circle();
        ZAY_RGB(panel, 0, 0, 0)
            panel.circle();

        // 불빛
        ZAY_RGBA(panel, 0, 255, 0, 255 * mBallAni)
            panel.circle();
    }
}

void RailBall::RenderInfo(ZayPanel& panel)
{
    // 정보
    ZAY_XYRR(panel, mStatus.mPosX, mStatus.mPosY, mBallSizeR, mBallSizeR)
    {
        // 이름
        if(0 < mBallAni || (panel.state(mUIName) & PS_Focused))
        {
            ZAY_RGB(panel, 255, 255, 255)
            ZAY_FONT(panel, 0.9)
                panel.text(panel.w() / 2, 0, mUIName, UIFA_CenterBottom);
        }

        // 적재된 이벤트량
        if(0 < mNextEvents.Count())
        {
            const String NextEvent = String::Format("%d", mNextEvents.Count());
            ZAY_RGB(panel, 255, 0, 255)
            ZAY_FONT(panel, 0.9)
                panel.text(panel.w() / 2, panel.h(), NextEvent, UIFA_CenterTop);
        }
    }
}

void RailBall::RenderWave(ZayPanel& panel)
{
    float WaveAni = 0;
    const uint64 WaveMsec = Platform::Utility::CurrentTimeMsec() - mWaveMsec;
    if(WaveMsec < 5 * mSlowVideo) // 0.5초
        WaveAni = WaveMsec / (5.0 * mSlowVideo);

    // 전파빛
    if(0 < WaveAni)
    ZAY_XYRR(panel, mStatus.mPosX, mStatus.mPosY, mWaveR * WaveAni, mWaveR * WaveAni)
    ZAY_RGBA(panel, 255, 255, 0, 64 * (1 - WaveAni))
        panel.circle();
}

void RailBall::RenderWaveBG(ZayPanel& panel)
{
    ZAY_XYRR(panel, mStatus.mPosX, mStatus.mPosY, mWaveR, mWaveR)
        panel.circle();
}

void RailBall::Tick(uint64 msec)
{
    // 자체 신호주기
    if(mWaveMsec + 50 * mSlowVideo < msec)
        RememberMe();

    // 주위셀에 전파중
    const uint64 WaveMsec = msec - mWaveMsec;
    if(WaveMsec < 5 * mSlowVideo)
        WaveFlush(mLiveEvents, mWaveR * WaveMsec / (5.0 * mSlowVideo));

    // 예약된 신호를 전파시작
    else if(0 < mNextEvents.Count())
    {
        mLiveEvents = ToReference(mNextEvents);
        mWaveMsec = msec;
    }
}

void RailBall::RememberMe()
{
    if(mNextEvents.Count() == 0)
    {
        auto& NewEvent = mNextEvents.AtAdding();
        NewEvent.mWrittenMsec = Platform::Utility::CurrentTimeMsec();
        NewEvent.mStatus = mStatus;
    }
}

void RailBall::OnWaveSniff(const BallEvents& events, float waveR)
{
    // 최소한 하나의 이벤트는 있어야 함
    if(events.Count() == 0)
        return;
    auto& Sender = events[0]; // 첫번째 이벤트는 항상 송신자

    // 전파송신자가 같은 그룹의 같은 번호 또는 상위번호여야 함
    if(mStatus.mRailCode != Sender.mStatus.mRailCode ||
        mStatus.mRailOrder > Sender.mStatus.mRailOrder)
        return;

    // 전파영향권에 닿지 않으면 실패
    if(waveR < Sender.DistanceTo(mStatus.mPosX, mStatus.mPosY))
        return;

    // 동일한 전파자의 같거나 오래된 전파는 실패
    const String SenderName = String::Format("%u-%f",
        Sender.mStatus.mRailCode, Sender.mStatus.mRailOrder);
    if(auto SavedEvent = mTotalEvents.Access(SenderName))
    if(Sender.mWrittenMsec <= SavedEvent->mWrittenMsec)
        return;

    // 전파내용 체크
    for(sint32 i = 0, iend = events.Count(); i < iend; ++i)
    {
        auto& CurEvent = events[i];

        // 릴레이 한계거리보다 멀면 스킵
        if(mRelayDist < CurEvent.DistanceTo(mStatus.mPosX, mStatus.mPosY))
            continue;

        // 기존에 수집된 이벤트라면 스킵
        const String CurName = String::Format("%u-%f",
            CurEvent.mStatus.mRailCode, CurEvent.mStatus.mRailOrder);
        if(auto SavedEvent = mTotalEvents.Access(CurName))
        if(CurEvent.mWrittenMsec <= SavedEvent->mWrittenMsec)
            continue;
        mTotalEvents(CurName) = CurEvent;

        // 다음으로 전파예약
        RememberMe(); // 첫번째 이벤트는 항상 자신
        mNextEvents.AtAdding() = CurEvent;
    }
}
