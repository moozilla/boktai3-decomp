#include "global.h"

struct P {
    u8 f00[0xb8];
    s32 st;
    u8 sub[0x14C - 0xBC];
    u32 w14c;
};
struct G {
    u8 f00[0x77C];
    s32 w;
};
extern struct G *gGameState;

void AnimData_SelectPaletteBank(void *, s32);
void Sound_PlaySong(s32);

s32 PlayerBarrier_TryActivate(struct P *p)
{
    struct P *q = p;
    if (gGameState->w <= 0)
        return 0;
    if (p->st != 3) {
        p->w14c = 0;
        p->st = 3;
    }
    AnimData_SelectPaletteBank(q->sub, 0x65);
    Sound_PlaySong(0x198);
    return 1;
}
