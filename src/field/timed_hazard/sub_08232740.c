#include "engine/shared/global.h"
struct S {
    u8 p[1];
    u8 b1;
    u8 b2;
    u8 q[3];
    u16 h;
    u16 g;
};
void TimedHazard_UpdateActivationDelay(struct S *s)
{
    if (s->h++ > s->g) {
        u32 o = 1;
        s->b2 = o;
        s->h = 0;
        s->b1 = o;
    }
}
