#include <boss.hpp>
#include "wavecomp.hpp"

WaveComp::WaveComp()
{
    mCellX = -1;
    mCellY = -1;
}

WaveComp::~WaveComp()
{
    MoveToCell(-1, -1);
}

typedef Array<WaveComp*> WaveCompCell;
typedef Map<WaveCompCell> WaveCompCells;
static WaveCompCells gCells;

void WaveComp::MoveToCell(sint32 x, sint32 y)
{
    // 현재 소속셀에서 제외
    if(mCellX != -1 && mCellY != -1)
    {
        auto& CurCell = gCells(String::Format("%d/%d", mCellX, mCellY));
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
    // 다시 소속셀에 등록
    if(x != -1 && y != -1)
    {
        mCellX = x;
        mCellY = y;
        auto& CurCell = gCells(String::Format("%d/%d", mCellX, mCellY));
        CurCell.AtAdding() = this;
    }
}

void WaveComp::WaveFlush(const BallEvents& events, float waveR)
{
    for(sint32 y = mCellY - 1, ymax = mCellY + 1; y <= ymax; ++y)
    for(sint32 x = mCellX - 1, xmax = mCellX + 1; x <= xmax; ++x)
    {
        auto& CurCell = gCells(String::Format("%d/%d", x, y));
        for(sint32 i = 0, iend = CurCell.Count(); i < iend; ++i)
        {
            auto CurBall = CurCell.At(i);
            if(this != CurBall)
                CurBall->OnWaveSniff(events, waveR);
        }
    }
}
