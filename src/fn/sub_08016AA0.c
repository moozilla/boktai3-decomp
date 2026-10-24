#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[8];
    u32 flags;
    u8 padc[20];
    struct Vec position;
    u8 pad28[12];
    u8 scaleX, scaleY;
    u16 angle;
};
struct Effect {
    u32 pad0;
    u8 state, entered, pad6, mode;
    u32 pad8, timer;
    u16 duration;
    u8 index, pad13[5];
    struct Vec position;
    u8 pad20[4];
    struct Sprite sprite;
};
void FloatingEffect_Project(struct Effect *);
void FloatingEffect_Init(struct Effect *);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
static inline u32 FloatingEffect_RandomAt(const u16 *table, u16 index)
{
    return table[index];
}
// confidence=low: rises for the first few frames, then sinks (shaking in mode 3) and moves to
// state 4 after 8 (mode 3: 9) frames.
void sub_08016AA0(u8 *m, struct Effect *p)
{
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
    }
    if (p->mode == 3) {
        if (p->timer <= 3)
            p->position.y += 0x40;
        else
            p->position.y -= 4;
    } else if (p->timer <= 4)
        p->position.y += 0x40;
    else
        p->position.y -= 8;
    FloatingEffect_Project(p);
    if ((p->mode == 3) && (p->timer > 3)) {
        const u16 *table = gUnk_0203B400;
        u32 *seed = &gUnk_03005308;
        u32 mask;
        u16 index = *seed + 1;
        mask = 0x3FF;
        index &= mask;
        *seed = index;
        {
            u32 bits = FloatingEffect_RandomAt(table, index);
            s32 x = (u16)p->sprite.position.x - 1;
            p->sprite.position.x = x + (bits & 3);
        }
        index = (index + 1) & mask;
        *seed = index;
        {
            u32 bits = FloatingEffect_RandomAt(table, index);
            s32 y = (u16)p->sprite.position.y - 1;
            p->sprite.position.y = y + (bits & 3);
        }
    }
    if (p->mode == 3) {
        if (p->timer <= 8)
            goto increment;
    } else if (p->timer <= 7)
        goto increment;
    {
        u32 nextState = 4;
        u32 zero = 0;
        p->state = nextState;
        p->entered = 1;
        p->timer = zero;
    }
    goto done;
increment:
    p->timer++;
done:;
}
