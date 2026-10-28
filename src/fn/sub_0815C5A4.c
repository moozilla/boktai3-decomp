#include "global.h"
#include "engine_globals.h"
#include "ram.h"

struct Vec {
    s16 x, y, z, w;
};

struct P {
    u8 filler[0x418];
    u8 state;
    u8 filler2[0x530 - 0x419];
    u16 h530;
    u8 filler3[0x5B8 - 0x532];
    u8 b5B8;
    u8 b5B9;
};

s32 EffectGroup_Spawn(void *, s32, s32, struct Vec *);
void Sound_PlaySong(s32);

static inline int randomWith(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

void PlayerCue_ShakeEffect(struct P *p, s32 a)
{
    struct Vec v;

    if (p->state == 3) {
        const u16 *t = gRandomTable;
        v.x = (randomWith(t) & 7) - 3;
        v.y = (randomWith(t) & 7) - 5;
        v.z = 0;
    } else if (p->state == 4) {
        const u16 *t = gRandomTable;
        v.x = (randomWith(t) & 7) - 3;
        v.y = (randomWith(t) & 7) - 3;
        v.z = 0;
    } else {
        const u16 *t = gRandomTable;
        v.x = (randomWith(t) & 15) - 7;
        v.y = -(randomWith(t) & 31);
        v.z = 0;
    }
    if (EffectGroup_Spawn(&p->b5B8, a, 0xE10, &v) >= 0) {
        if (p->b5B9 == 1)
            Sound_PlaySong(0x191);
        else if (p->b5B9 == 2)
            Sound_PlaySong(0x133);
        else if (p->b5B9 == 3)
            Sound_PlaySong(0x133);
    }
    p->h530 = 0;
}
