#include <boss.hpp>
#include "car.hpp"

#include <resource.hpp>

Car::Car()
{
    mCarAni = 0;
}

Car::~Car()
{
}

static sint32 gFocusedCar = -1;
sint32 Car::FocusedCar()
{
    return gFocusedCar;
}

static sint32 gNextCarID = 0;
void Car::Init(double x, double y)
{
    mCarID = gNextCarID;
    mPosX = x;
    mPosY = y;
    MoveToCell(((sint32) x) / mWaveR, ((sint32) y) / mWaveR);
    gNextCarID++;
    gFocusedCar = mCarID; // 새로 만들어진 차량으로 포커스이동

    mUIName = String::Format("car-%d", mCarID);
    mCarAni = 0;
}

void Car::RunOnce()
{
    // 핸들을 서서히 되돌리기
    if(0.2 < mTireAngle) mTireAngle -= 0.2;
    else if(mTireAngle < -0.2) mTireAngle += 0.2;
    else mTireAngle = 0;

    // 가속
    mSpeed = Math::MinF(mSpeed + 0.1, 4);

    // 수동조작시 자율주행 무효화
    mTargetCode = 0;
}

void Car::BreakOnce()
{
    // 감속
    mSpeed *= 0.95;

    // 수동조작시 자율주행 무효화
    mTargetCode = 0;
}

void Car::RotateOnce(bool left)
{
    if(left)
        mTireAngle = Math::MaxF(-20, mTireAngle - 0.5);
    else mTireAngle = Math::MinF(mTireAngle + 0.5, 20);

    // 수동조작시 자율주행 무효화
    mTargetCode = 0;
}

void Car::RenderCar(ZayPanel& panel)
{
    mCarAni = Math::Max(0, mCarAni - 1);

    // 차량
    ZAY_XYRR_UI(panel, mPosX, mPosY, mCarR, mCarR, mUIName,
        ZAY_GESTURE_TXY(t, x, y, this)
        {
            static Point OldPos;
            if(t == GT_Pressed)
            {
                gFocusedCar = mCarID; // 차량을 포커싱
                OldPos = Point(x, y);
            }
            else if(t == GT_InDragging || t == GT_OutDragging)
            {
                mPosX += x - OldPos.x;
                mPosY += y - OldPos.y;
                MoveToCell(((sint32) mPosX) / mWaveR, ((sint32) mPosY) / mWaveR);
                OldPos = Point(x, y);
            }
        })
    {
        const float One = mCarR * 0.8f;
        const double CarCos = Math::Cos(Math::ToRadian(mCarAngle));
        const double CarSin = Math::Sin(Math::ToRadian(mCarAngle));

        // 좌표준비
        Points CarFrame;
        CarFrame.AtAdding() = Point(One / 5, -One);
        CarFrame.AtAdding() = Point(One / 5, One / 5 - One);
        CarFrame.AtAdding() = Point(One / 3, One * 2 / 5 - One);
        CarFrame.AtAdding() = Point(One / 3, One - One / 4);
        CarFrame.AtAdding() = Point(One / 6, One - One / 4);
        CarFrame.AtAdding() = Point(One / 6, One);
        CarFrame.AtAdding() = Point(-One / 6, One);
        CarFrame.AtAdding() = Point(-One / 6, One - One / 4);
        CarFrame.AtAdding() = Point(-One / 3, One - One / 4);
        CarFrame.AtAdding() = Point(-One / 3, One * 2 / 5 - One);
        CarFrame.AtAdding() = Point(-One / 5, One / 5 - One);
        CarFrame.AtAdding() = Point(-One / 5, -One);
        for(sint32 i = 0, iend = CarFrame.Count(); i < iend; ++i)
        {
            const Point CurPos = CarFrame.At(i);
            CarFrame.At(i).x = CurPos.x * CarCos - CurPos.y * CarSin;
            CarFrame.At(i).y = CurPos.x * CarSin + CurPos.y * CarCos;
            CarFrame.At(i) += Point(panel.w() / 2, panel.h() / 2);
        }
        Points Sticks;
        Sticks.AtAdding() = Point(-One / 2, One / 8 - One);
        Sticks.AtAdding() = Point(One / 2, One / 8 - One);
        Sticks.AtAdding() = Point(-One / 2, One - One / 8);
        Sticks.AtAdding() = Point(One / 2, One - One / 8);
        for(sint32 i = 0, iend = Sticks.Count(); i < iend; ++i)
        {
            const Point CurPos = Sticks.At(i);
            Sticks.At(i).x = CurPos.x * CarCos - CurPos.y * CarSin;
            Sticks.At(i).y = CurPos.x * CarSin + CurPos.y * CarCos;
            Sticks.At(i) += Point(panel.w() / 2, panel.h() / 2);
        }

        // 프레임바디
        ZAY_RGB(panel, 240, 240, 255)
        ZAY_RGB_IF(panel, 80, 100, 120, mCarID == gFocusedCar)
        ZAY_RGB_IF(panel, 90, 90, 90, panel.state(mUIName) & PS_Focused)
            panel.polygon(CarFrame);
        if(0 < mCarAni)
        ZAY_RGBA(panel, 255, 255, 0, 255 * mCarAni / 50)
            panel.polygon(CarFrame);

        // 외곽선
        ZAY_RGB(panel, 0, 0, 64)
        {
            // 바디외곽선
            const Point FirstPoint = CarFrame[0];
            CarFrame.AtAdding() = FirstPoint;
            panel.polyline(CarFrame, 1);
            // 타이어스틱
            panel.line(Sticks[0], Sticks[1], 1);
            panel.line(Sticks[2], Sticks[3], 1);

            // 타이어
            const double TireCos = Math::Cos(Math::ToRadian(mCarAngle + mTireAngle));
            const double TireSin = Math::Sin(Math::ToRadian(mCarAngle + mTireAngle));
            for(sint32 i = 0; i < 4; ++i)
            {
                Points Tire;
                Tire.AtAdding() = Point(One / 12, -One / 5);
                Tire.AtAdding() = Point(One / 12, One / 5);
                Tire.AtAdding() = Point(-One / 12, One / 5);
                Tire.AtAdding() = Point(-One / 12, -One / 5);
                for(sint32 j = 0, jend = Tire.Count(); j < jend; ++j)
                {
                    const Point CurPos = Tire.At(j);
                    if(i < 2) // 앞타이어
                    {
                        Tire.At(j).x = CurPos.x * TireCos - CurPos.y * TireSin;
                        Tire.At(j).y = CurPos.x * TireSin + CurPos.y * TireCos;
                    }
                    else
                    {
                        Tire.At(j).x = CurPos.x * CarCos - CurPos.y * CarSin;
                        Tire.At(j).y = CurPos.x * CarSin + CurPos.y * CarCos;
                    }
                    Tire.At(j) += Sticks[i];
                }
                panel.polygon(Tire);
            }
        }
    }
}

void Car::RenderCarBG(ZayPanel& panel)
{
    ZAY_XYRR(panel, mPosX, mPosY, mCarR, mCarR)
        panel.circle();
}

void Car::Tick(uint64 msec)
{
    // 전진
    mCarAngle = Math::Mod(360 + mCarAngle + mTireAngle * mSpeed / 20, 360);
    const double CarCos = Math::Cos(Math::ToRadian(mCarAngle));
    const double CarSin = Math::Sin(Math::ToRadian(mCarAngle));
    mPosX += mSpeed * CarSin;
    mPosY -= mSpeed * CarCos;
    MoveToCell(((sint32) mPosX) / mWaveR, ((sint32) mPosY) / mWaveR);

    // 자연감속
    mSpeed = Math::MaxF(0, mSpeed - 0.02 - 0.04 * Math::AbsF(mTireAngle) / 20);

    // 자율주행
    if(0 < mTargetCode)
    {
        // 타게팅 루프
        for(sint32 i = 0, iend = mTargetEvents.Count(); i < iend; ++i)
        {
            auto& CurEvent = mTargetEvents[i];
            if(CurEvent.mStatus.mRailCode == mTargetCode && CurEvent.mStatus.mRailOrder == mTargetOrder)
            {
                // 방향설정
                const double TargetAngle =
                    Math::Mod(Math::ToDegree(Math::Atan(CurEvent.mStatus.mPosX - mPosX, CurEvent.mStatus.mPosY - mPosY)) + 90, 360);
                const double CarAngle = Math::Mod(360 + mCarAngle + mTireAngle, 360);
                double AngleAdjust = 0;
                if(CarAngle < 180)
                {
                    if(Math::AbsF(TargetAngle - CarAngle) < Math::AbsF(TargetAngle - (CarAngle + 360)))
                        AngleAdjust = TargetAngle - CarAngle;
                    else AngleAdjust = TargetAngle - (CarAngle + 360);
                }
                else if(Math::AbsF(TargetAngle - CarAngle) < Math::AbsF(TargetAngle - (CarAngle - 360)))
                    AngleAdjust = TargetAngle - CarAngle;
                else AngleAdjust = TargetAngle - (CarAngle - 360);

                // 자율주행 포기조건
                if(AngleAdjust < -30 || 30 < AngleAdjust)
                    mTargetCode = 0;
                // 자율주행 판단
                else if(CurEvent.DistanceTo(mPosX, mPosY) < 15)
                {
                    // 타게팅 전이
                    if(i + 1 < iend && mTargetEvents[i + 1].mStatus.mRailCode == mTargetCode)
                        mTargetOrder = mTargetEvents[i + 1].mStatus.mRailOrder;
                    // 자율주행 완료
                    else mTargetCode = 0;
                }
                // 자율주행 진행
                else
                {
                    const uint32 OldTargetCode = mTargetCode;

                    // 속도유지
                    if(1 < mSpeed)
                        BreakOnce();
                    else RunOnce();

                    // 핸들조정
                    if(0.5 <= AngleAdjust)
                        RotateOnce(false);
                    else if(AngleAdjust <= -0.5)
                        RotateOnce(true);

                    // 자율주행 유지
                    mTargetCode = OldTargetCode;
                }
                break;
            }
        }
    }
}

void Car::OnWaveSniff(const BallEvents& events, float waveR)
{
    // 최소한 하나의 이벤트는 있어야 함
    if(events.Count() == 0)
        return;
    auto& Sender = events[0]; // 첫번째 이벤트는 항상 송신자

    // 전파영향권에 닿지 않으면 실패
    if(waveR < Sender.DistanceTo(mPosX, mPosY))
        return;

    // 자율주행
    if(mTargetCode == 0)
    {
        mTargetCode = Sender.mStatus.mRailCode;
        mTargetOrder = Sender.mStatus.mRailOrder;
    }
    mTargetEvents = events;

    // 애니메이션
    mCarAni = 50;
}
