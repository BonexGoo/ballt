#include <boss.hpp>
#include "car.hpp"

#include <resource.hpp>

Car::Car()
{
    mCellX = -1;
    mCellY = -1;
}

Car::~Car()
{
    MoveToCell(-1, -1);
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
    gNextCarID++;

    gFocusedCar = mCarID; // 새로 만들어진 차량으로 포커스이동
    MoveToCell(((sint32) x) / mWaveR, ((sint32) y) / mWaveR);
}

void Car::MoveToCell(sint32 x, sint32 y)
{
}

void Car::RunOnce()
{
    mSpeed = Math::MinF(mSpeed + 0.1, 5);
}

void Car::BreakOnce()
{
    mSpeed *= 0.95;
}

void Car::RotateOnce(bool left)
{
    if(left)
        mTireAngle = Math::MaxF(-20, mTireAngle - 0.5);
    else mTireAngle = Math::MinF(mTireAngle + 0.5, 20);
}

void Car::RenderCar(ZayPanel& panel)
{
    // 차량
    const String UIName = String::Format("car-%d", mCarID);
    ZAY_XYRR_UI(panel, mPosX, mPosY, mCarR, mCarR, UIName,
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
                OldPos = Point(x, y);
                MoveToCell(((sint32) mPosX) / mWaveR, ((sint32) mPosY) / mWaveR);
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
        ZAY_RGB_IF(panel, 90, 90, 90, panel.state(UIName) & PS_Focused)
            panel.polygon(CarFrame);
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

    // 자연감속
    mSpeed = Math::MaxF(0, mSpeed - 0.02 - 0.04 * Math::AbsF(mTireAngle) / 20);
}
