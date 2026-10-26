#include "global.h"

struct G {
    u8 f[0x12];
    s16 a;
};
extern struct G *gGameState;
struct S {
    u8 f[8];
    u8 a;
    u8 b;
};
void TextState_ResetPlaybackMode(struct S *p)
{
    s32 v;
    p->a = 0;
    v = gGameState->a;
    p->b = v;
}
