#include "global.h"
struct S {
    u8 p[0x1da];
    u16 h;
    u32 a;
};
void SolarBomb_SetState(struct S *s, u32 v)
{
    s->a = v;
    s->h = 0;
}
