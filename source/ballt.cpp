#include <boss.hpp>
#include "ballt.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("balltView", balltData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 틱전달
        uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        m->mBalls.AccessByCallback(
            [](const MapPath* path, RailBall* ball, payload data)->void
            {
                ball->Tick(*((uint64*) data));
            }, &CurMsec);
        m->mCars.AccessByCallback(
            [](const MapPath* path, Car* car, payload data)->void
            {
                car->Tick(*((uint64*) data));
            }, &CurMsec);

        // 차량이동
        const sint32 CurCarIndex = Car::FocusedCar();
        if(CurCarIndex != -1 && CurCarIndex < m->mCars.Count())
        {
            auto& CurCar = *m->mCars.AccessByOrder(CurCarIndex);
            if(m->mKeyLeft | m->mKeyUp | m->mKeyRight | m->mKeyDown)
            {
                // 전진
                if(m->mKeyUp && !m->mKeyDown)
                    CurCar.RunOnce();
                // 브레이크
                if(m->mKeyDown && !m->mKeyUp)
                    CurCar.BreakOnce();
                // 선회
                if(m->mKeyLeft + m->mKeyRight == 1)
                    CurCar.RotateOnce(m->mKeyLeft);
            }
        }
        m->invalidate(2);
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_KeyPress || type == NT_KeyRelease)
    {
        const sint32 KeyCode = sint32o(in).ConstValue();
        const bool KeyPressed = (type == NT_KeyPress);
        switch(KeyCode)
		{
        case 0x01000012: case 'A': m->mKeyLeft = KeyPressed; break;
        case 0x01000013: case 'W': m->mKeyUp = KeyPressed; break;
        case 0x01000014: case 'D': m->mKeyRight = KeyPressed; break;
        case 0x01000015: case 'S': m->mKeyDown = KeyPressed; break;
        }
    }
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
    if(type == GT_Pressed)
    {
        auto& NewBall = m->mBalls.AtAdding();
        NewBall.Init(x, y);
    }
    else if(type == GT_ExtendPress)
    {
        auto& NewCar = m->mCars.AtAdding();
        NewCar.Init(x, y);
    }
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    // 배경색
    ZAY_RGB(panel, 64, 64, 64)
        panel.fill();

    // 레일볼의 전파권역
    ZAY_RGB(panel, 60, 60, 60)
        m->mBalls.AccessByCallback(
            [](const MapPath* path, RailBall* ball, payload data)->void
            {
                ball->RenderWaveBG(*((ZayPanel*) data));
            }, &panel);

    // 차량의 그림자
    ZAY_RGBA(panel, 200, 200, 255, 30)
        m->mCars.AccessByCallback(
            [](const MapPath* path, Car* car, payload data)->void
            {
                car->RenderCarBG(*((ZayPanel*) data));
            }, &panel);

    // 어플리케이션 정보
    ZAY_RGBA(panel, 255, 255, 255, 64)
        m->RenderInfo(panel);

    // 레일볼의 전파
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderWave(*((ZayPanel*) data));
        }, &panel);

    // 레일볼
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderBall(*((ZayPanel*) data));
        }, &panel);

    // 레일볼의 정보
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderInfo(*((ZayPanel*) data));
        }, &panel);

    // 차량
    m->mCars.AccessByCallback(
        [](const MapPath* path, Car* car, payload data)->void
        {
            car->RenderCar(*((ZayPanel*) data));
        }, &panel);

    // 그룹버튼
    ZAY_XYWH(panel, 10, 10, 100, 25)
        RailBall::RenderRailCode(panel);
}

balltData::balltData()
{
    mKeyLeft = false;
    mKeyUp = false;
    mKeyRight = false;
    mKeyDown = false;
}

balltData::~balltData()
{
}

void balltData::RenderInfo(ZayPanel& panel)
{
    // 버전
    ZAY_XYWH(panel, panel.w() - 10 - 200, 10, 200, 25)
    {
        panel.rect(2);
        panel.text(String::Format("Build: %s %s", __DATE__, __TIME__));
    }

    // 축적
    ZAY_XYWH(panel, panel.w() - 10 - 20, 45, 20, 20)
    {
        ZAY_XYWH(panel, -2, 0, 2, 10)
            panel.fill();
        ZAY_XYWH(panel, panel.w(), 0, 2, 10)
            panel.fill();
        ZAY_XYWH(panel, 0, 4, panel.w(), 2)
            panel.fill();
        ZAY_XYWH(panel, 0, 10, panel.w(), 10)
        ZAY_FONT(panel, 0.8)
            panel.text("2m");
    }

    // 사용법
    chars Help[3] = {
        "Click Mouse-L to add a ball",
        "Click Mouse-R to add a car",
        "Car driving: (W)(A)(S)(D) or (↑)(←)(↓)(→)"};
    for(sint32 i = 0; i < 3; ++i)
    {
        ZAY_XYWH(panel, panel.w() - 10 - 20, 70 + 15 * i, 20, 15)
        ZAY_FONT(panel, 0.8)
            panel.text(panel.w(), panel.h() / 2, Help[i], UIFA_RightMiddle);
    }
}
