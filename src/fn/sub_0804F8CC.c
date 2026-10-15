#include "repeated_effect.h"
#define RepeatedEffect_TickLowerTransition sub_0804F8CC
void Sound_PlaySong(s32);

void RepeatedEffect_TickLowerTransition(struct RepeatedEffect *p)
{
    u16 *c = &p->stateFrame;
    u32 v;
    if (*c == 0)
        Sound_PlaySong(0x19f);
    v = *c + 1;
    *c = v;
    if ((u16)v <= 7) {
        p->sprite.frame = 3;
    } else if ((u16)v <= 0xf) {
        p->sprite.frame = 4;
    } else {
        p->sprite.frame = 5;
        sub_0804F7D0(p, 1, sub_0804F888);
    }
}
