#include "global.h"
struct P {
    u8 f[0x43E9];
    u8 v;
};
void PauseMenu_ClearSpriteFlags(struct P *, s32, s32);
void PauseMenu_SetSpritePosition(struct P *, s32, s32, s32);
void PauseMenu_LoadSpriteKeyframe(struct P *, s32, s32);
void PauseMenu_SetConfirmationIndicator(struct P *p, s32 b)
{
    s32 r;
    PauseMenu_ClearSpriteFlags(p, 7, 1);
    PauseMenu_SetSpritePosition(p, 7, 0, 0);
    p->v = b;
    if ((u8)b)
        r = 0x4b;
    else
        r = 0x4c;
    PauseMenu_LoadSpriteKeyframe(p, 7, r);
}
