#include <boss.hpp>
#include "ballt.hpp"

#include <resource.hpp>

ZAY_DECLARE_VIEW_CLASS("balltView", balltData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 볼의 루프
        uint64 CurMsec = Platform::Utility::CurrentTimeMsec();
        m->mBalls.AccessByCallback(
            [](const MapPath* path, RailBall* ball, payload data)->void
            {
                ball->Tick(*((uint64*) data));
            }, &CurMsec);
        m->invalidate(2);
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
    if(type == GT_Pressed)
    {
        auto& NewBall = m->mBalls.AtAdding();
        NewBall.Init(x, y);
    }
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    // 배경색
    ZAY_RGB(panel, 64, 64, 64)
        panel.fill();
    // 전파배경
    ZAY_RGB(panel, 60, 60, 60)
        m->mBalls.AccessByCallback(
            [](const MapPath* path, RailBall* ball, payload data)->void
            {
                ball->RenderWaveBG(*((ZayPanel*) data));
            }, &panel);

    // 정보
    ZAY_RGBA(panel, 255, 255, 255, 64)
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
    }

    // 전파그리기
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderWave(*((ZayPanel*) data));
        }, &panel);

    // 볼그리기
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderBall(*((ZayPanel*) data));
        }, &panel);

    // 정보그리기
    m->mBalls.AccessByCallback(
        [](const MapPath* path, RailBall* ball, payload data)->void
        {
            ball->RenderInfo(*((ZayPanel*) data));
        }, &panel);

    // 그룹버튼
    ZAY_XYWH(panel, 10, 10, 100, 25)
        RailBall::RenderRailCode(panel);
}

balltData::balltData()
{
}

balltData::~balltData()
{
}
