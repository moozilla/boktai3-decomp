#include "repeated_effect.h"
#define RepeatedEffect_TickLowerRecovery sub_0804F91C
void Sound_PlaySong(s32);

void RepeatedEffect_TickLowerRecovery(struct RepeatedEffect *p)
{
    u16 *c = &p->stateFrame;
    u32 v;
    if (*c == 0)
        Sound_PlaySong(0x1a0);
    v = *c + 1;
    *c = v;
    if ((u16)v <= 7) {
        p->sprite.frame = 4;
    } else if ((u16)v <= 0xf) {
        p->sprite.frame = 3;
    } else {
        p->sprite.frame = 2;
        Sound_PlaySong(0x1a0);
        sub_0804F7D0(p, 0, sub_0804FB48);
    }
}
