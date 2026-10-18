#include "global.h"

void SceneActorState_StepTableByte(u8 *p, u8 *tbl)
{
    u16 *cur = (u16 *)(p + 0x3b2);
    u16 old = *cur;
    u8 *col;
    u32 row;
    *(u16 *)(p + 0x3b4) = old;
    row = p[0x3b0];
    col = p + 0x3b1;
    {
        u32 idx = row * 8 + (u32)tbl;
        idx += *col;
        *cur = *(u8 *)idx;
    }
    (*col)++;
    if (*col > 7)
        *col = 0;
}
