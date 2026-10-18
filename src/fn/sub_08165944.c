#include "global.h"
struct S {
    u8 f[0x428];
    u16 a;
    u16 b;
};
struct P {
    u8 f[0xA40];
    struct S *s;
};
void Hud_DrawDefaultHealthMeter(struct P *, s32, s32, s32);
void Hud_DrawDefaultEnergyMeter(struct P *, s32, s32, s32, s32, s32);
void Hud_DrawDefaultResourceCounts(s32, s32, s32, s32, s32);
void Hud_DrawStatusMetersAndCounts(struct P *p, s32 b, s32 c)
{
    Hud_DrawDefaultHealthMeter(p, 1, b, c);
    c += 2;
    Hud_DrawDefaultEnergyMeter(p, 1, b, c, p->s->a, p->s->b);
    Hud_DrawDefaultResourceCounts(8, 4, 8, 3, 0);
}
