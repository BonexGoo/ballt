#include <boss.hpp>
#include "railball.hpp"

#include <resource.hpp>

typedef Array<RailBall*> BallCell;
typedef Map<BallCell> BallLand;
BallLand gBallLand;

RailBall::RailBall()
{
    mLifeMsec = 0;
    mWaveMsec = 0;
    mCellX = -1;
    mCellY = -1;
}

RailBall::~RailBall()
{
    BindCell(-1, -1);
}

uint32 gRailCode = 1;
float gRailOrder = 1;
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

    mLifeMsec = Platform::Utility::CurrentTimeMsec();
    mWaveMsec = 0;

    BindCell(((sint32) x) / mWaveR, ((sint32) y) / mWaveR);
    gRailOrder += 1;
}

void RailBall::BindCell(sint32 x, sint32 y)
{
    // 소속셀에서 제외
    if(mCellX != -1 && mCellY != -1)
    {
        auto& CurCell = gBallLand(String::Format("%d/%d", mCellX, mCellY));
        for(sint32 i = 0, iend = CurCell.Count(); i < iend; ++i)
        {
            auto CurBall = CurCell.At(i);
            if(this == CurBall)
            {
                CurCell.SubtractionSection(i);
                break;
            }
        }
    }

    // 소속셀에 등록
    if(x != -1 && y != -1)
    {
        mCellX = x;
        mCellY = y;
        auto& CurCell = gBallLand(String::Format("%d/%d", mCellX, mCellY));
        CurCell.AtAdding() = this;
    }
}

void RailBall::RenderBall(ZayPanel& panel)
{
    float AniValue = 0;
    const uint64 LifeMsec = Platform::Utility::CurrentTimeMsec() - mLifeMsec;
    if(LifeMsec < 200 && 0 < mWaveMsec)
        AniValue = 1 - LifeMsec / 200.0;
    const float SizeR = 5 + 5 * AniValue;

    ZAY_XYRR(panel, mStatus.mPosX, mStatus.mPosY, SizeR, SizeR)
    {
        // 틀
        ZAY_INNER(panel, -1)
        ZAY_RGB(panel, 255, 255, 255)
            panel.circle();
        ZAY_RGB(panel, 0, 0, 0)
            panel.circle();

        // 불빛
        ZAY_RGBA(panel, 0, 255, 0, 255 * AniValue)
            panel.circle();
    }
}

void RailBall::RenderInfo(ZayPanel& panel)
{
    float AniValue = 0;
    const uint64 LifeMsec = Platform::Utility::CurrentTimeMsec() - mLifeMsec;
    if(LifeMsec < 200 && 0 < mWaveMsec)
        AniValue = 1 - LifeMsec / 200.0;
    const float SizeR = 5 + 5 * AniValue;

    // 이름
    const String UIName = String::Format("%u-%u",
        mStatus.mRailCode, (uint32) mStatus.mRailOrder);
    ZAY_XYRR_UI(panel, mStatus.mPosX, mStatus.mPosY, SizeR, SizeR, UIName,
        ZAY_GESTURE_TXY(t, x, y, this)
        {
            static Point OldPos;
            if(t == GT_Pressed)
                OldPos = Point(x, y);
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                mStatus.mPosX += x - OldPos.x;
                mStatus.mPosY += y - OldPos.y;
                BindCell(((sint32) mStatus.mPosX) / mWaveR, ((sint32) mStatus.mPosY) / mWaveR);
                OldPos = Point(x, y);
            }
        })
    {
        if(0 < AniValue || (panel.state(UIName) & PS_Focused))
        {
            ZAY_RGB(panel, 255, 0, 0)
            ZAY_FONT(panel, 0.9)
                panel.text(panel.w() / 2, 0, UIName, UIFA_CenterBottom);

            const String EventCount = String::Format("%d", mLiveEvents.Count());
            ZAY_RGB(panel, 0, 0, 0)
            ZAY_FONT(panel, 0.7)
                panel.text(panel.w() / 2, panel.h() / 2, EventCount);
        }
    }
}

void RailBall::RenderWave(ZayPanel& panel)
{
    float AniValue = 0;
    const uint64 WaveMsec = Platform::Utility::CurrentTimeMsec() - mWaveMsec;
    if(WaveMsec < 500)
        AniValue = WaveMsec / 500.0;

    // 전파
    if(0 < AniValue)
    ZAY_XYRR(panel, mStatus.mPosX, mStatus.mPosY, 5, 5)
    ZAY_INNER(panel, 5 - mWaveR * AniValue)
    ZAY_RGBA(panel, 255, 255, 0, 64 * (1 - AniValue))
        panel.circle();
}

void RailBall::Tick(uint64 msec)
{
    // 자체 신호주기
    if(mLifeMsec + 5000 < msec)
        SendMe();

    // 주위셀에 전파중
    const uint64 WaveMsec = msec - mWaveMsec;
    if(WaveMsec < 500)
    {
        const float CurWaveR = mWaveR * WaveMsec / 500.0;
        for(sint32 y = mCellY - 1, ymax = mCellY + 1; y <= ymax; ++y)
        for(sint32 x = mCellX - 1, xmax = mCellX + 1; x <= xmax; ++x)
        {
            auto& CurCell = gBallLand(String::Format("%d/%d", x, y));
            for(sint32 i = 0, iend = CurCell.Count(); i < iend; ++i)
            {
                auto CurBall = CurCell.At(i);
                if(this != CurBall)
                    CurBall->WaveTest(*this, CurWaveR);
            }
        }
    }
    // 예약된 신호를 전파시작
    else if(0 < mNextEvents.Count())
    {
        mLifeMsec = msec;
        mWaveMsec = msec;
        mLiveEvents = ToReference(mNextEvents);
    }
}

void RailBall::SendMe()
{
    if(mNextEvents.Count() == 0)
    {
        auto& NewEvent = mNextEvents.AtAdding();
        NewEvent.mStatus = mStatus;
    }
}

void RailBall::WaveTest(const RailBall& sender, float waveR)
{
    // 같은 그룹의 상위번호가 아니면 실패
    if(mStatus.mRailCode != sender.mStatus.mRailCode ||
        mStatus.mRailOrder >= sender.mStatus.mRailOrder)
        return;

    // 전파영향권에 닿지 않으면 실패
    const float WaveDist = Math::Distance(mStatus.mPosX, mStatus.mPosY,
        sender.mStatus.mPosX, sender.mStatus.mPosY);
    if(waveR < WaveDist)
        return;

    // 동일한 전파자의 동일한 전파는 실패
    const String SenderName = String::Format("%u-%f",
        sender.mStatus.mRailCode, sender.mStatus.mRailOrder);
    if(mTestedWaveMsec(SenderName) == sender.mWaveMsec)
        return;
    mTestedWaveMsec(SenderName) = sender.mWaveMsec;

    // 전파내용 전수확인
    for(sint32 i = 0, iend = sender.mLiveEvents.Count(); i < iend; ++i)
    {
        auto& SrcEvent = sender.mLiveEvents[i];
        const float BallDist = Math::Distance(mStatus.mPosX, mStatus.mPosY,
            SrcEvent.mStatus.mPosX, SrcEvent.mStatus.mPosY);
        if(mRelayDist < BallDist) // 릴레이 한계거리보다 멀면 스킵
            continue;

        // 기존 예약된 이벤트와 같은 것은 추가하지 않음
        bool SameEvent = false;
        for(sint32 j = 0, jend = mNextEvents.Count(); j < jend; ++j)
        {
            auto& DstEvent = mNextEvents[j];
            if(DstEvent.mStatus.mRailCode == SrcEvent.mStatus.mRailCode &&
                DstEvent.mStatus.mRailOrder == SrcEvent.mStatus.mRailOrder)
            {
                SameEvent = true;
                break;
            }
        }
        if(SameEvent) continue;

        // 이벤트 예약
        SendMe();
        auto& NewEvent = mNextEvents.AtAdding();
        NewEvent.mStatus = SrcEvent.mStatus;
    }
}
