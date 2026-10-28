#include "global.h"
#include "engine_globals.h"
extern u8 gUnk_0203B400[];
s32 Mod(s32, s32);
struct S {
    u8 f[0x1e];
    u16 a;
    u8 g[0xc];
    u16 b;
    u16 c;
};
void Weather_SetRandomDuration(struct S *s)
{
    s->a = s->b;
    if (s->c != 0) {
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        s->a += Mod(*(u16 *)(gUnk_0203B400 + gRandomIndex * 2), s->c);
    }
    if (s->a <= 9)
        s->a = 10;
}
