#include "global.h"

struct S {
    u8 filler[0xb6];
    u16 h;
    u8 filler2[0xdc - 0xb8];
    u32 k;
};
void TetherActor_AddStatusFlags(u32, u32);
void TetherActor_RemoveStatusFlags(u32);

void sub_081029C0(struct S *s)
{
    if ((u16)(s->h - 3) > 1) {
        TetherActor_AddStatusFlags(7, s->k);
    } else {
        TetherActor_AddStatusFlags(3, s->k);
        TetherActor_RemoveStatusFlags(4);
    }
}
