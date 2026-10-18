#include "global.h"
struct S {
    u8 p[0xbc];
    u32 a;
    u16 pad;
    u16 h;
};
void SpritePulseEffect_OnAnimationEvent(u32 a, u32 b, struct S *s)
{
    s->a = 1;
    s->h = 0;
}
