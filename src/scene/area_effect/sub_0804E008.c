#include "global.h"

struct S {
    u8 filler[0x876];
    s16 v;
};
extern struct S *gGameState;
s32 Div(s32, s32);

s32 AreaEffect_ScaleResourceGain(s32 x)
{
    s32 r = Div(x * gGameState->v, 10);
    if (r <= 0)
        r = 1;
    return r;
}
