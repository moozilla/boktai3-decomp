#include "global.h"
struct S {
    u8 p[0x70];
    u16 h;
    u8 q[0xca];
    u32 a;
};
void ActorGlyphTrail_SetState(struct S *s, u32 v)
{
    s->a = v;
    s->h = 0;
}
