#include "global.h"
void Bg_ClearTilemap(s32);
void StageResults_HideSprites(void *);
u32 GameState_GetResultIndex(void);
void Script_Call(u32, void *);
void StageResults_FinishDisplay(u8 *s)
{
    u32 loc[3];
    u32 *p;
    u32 *q;
    u32 m;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    StageResults_HideSprites(s);
    p = (u32 *)(s + 0x1210);
    if (*p != 0) {
        loc[0] = GameState_GetResultIndex();
        m = 0xFFFF0000;
        loc[1] = (loc[1] & m) | 1;
        q = &loc[1];
        q[1] = (u32)loc;
        Script_Call(*p, q);
    }
}
