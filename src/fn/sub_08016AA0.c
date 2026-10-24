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
// confidence=low: rises for the first few frames, then sinks (shaking in mode 3) and moves to
// state 4 after 8 (mode 3: 9) frames.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The shaking jitter differs: the ROM keeps the table in r8 and computes x - 1 before adding the random bits.
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
        u16 index = ((*seed) + 1) & 0x3FF;
        *seed = index;
        {
            typeof(table[index]) v1 = table[index];
            p->sprite.position.x += (v1 & 3) - 1;
        }
        index = (index + 1) & 0x3FF;
        *seed = index;
        p->sprite.position.y += (table[index] & 3) - 1;
    }
    if ((p->mode == 3) ? (p->timer > 8) : (p->timer > 7)) {
        do {
            p->state = 4;
        } while (0);
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08016AA0);
#endif
