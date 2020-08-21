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

    // 버전정보
    ZAY_XYWH(panel, panel.w() - 10 - 200, 10, 200, 25)
    ZAY_RGBA(panel, 255, 255, 255, 64)
    {
        panel.rect(2);
        panel.text(String::Format("Build: %s %s", __DATE__, __TIME__));
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
