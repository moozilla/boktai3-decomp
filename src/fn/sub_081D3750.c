#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.5%
// Only register allocation differs: the original keeps actor in r6 and &p->angle in r7; here they swap.
// The redundant if (p) is a permuter find that fixes block order.

struct TurnSprite {
    u8 pad0[0x18];
    s16 x, y, z;
};
struct TurnTarget {
    s16 x, y, z;
};
struct TurnParticle {
    u8 state;
    u8 pad1[2];
    u8 speed;
    u8 angle;
    s8 turn;
    u8 pad6[6];
    s16 vx;
    u8 padE[2];
    s16 vz;
    u8 pad12[0xA];
    struct TurnTarget *target;
    struct TurnSprite sprite_at20;
};
extern const s16 gUnk_086149C4[];
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Math_Atan2(s32, s32);
static inline u32 randomBits(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return ((const u16 *)0x0203B400)[gUnk_03005308] & 0x3F;
}
void sub_081D3750(u8 *actor, struct TurnParticle *p)
{
    struct TurnTarget *target = p->target;
    struct TurnSprite *sprite = &p->sprite_at20;
    u8 *angle = &p->angle;
    s32 diff;
    s32 abs;
    s32 vx;
    s32 height;
    u8 current;
    s32 heading = Math_Atan2(target->x - sprite->x, target->z - sprite->z);
    if (p) {
        current = p->angle;
        diff = heading - current;
    } else {
        current = p->angle;
        diff = heading - current;
    }
    if (p->turn < 0) {
        abs = diff;
        if (diff < 0)
            abs = -diff;
        if (abs > actor[0x1e])
            *angle = current + actor[0x1e];
        else
            *angle = current + diff;
        p->turn++;
        if (p->turn == 0)
            p->turn = randomBits() + 0x40;
    } else {
        abs = diff;
        if (diff < 0)
            abs = -diff;
        if (abs > actor[0x1e])
            *angle = current - actor[0x1e];
        else
            *angle = current - diff;
        p->turn--;
        if (p->turn == 0)
            p->turn = -(s32)randomBits() - 0x40;
    }
    vx = (gUnk_086149C4[(*angle + 0x40) & 0xFF] * p->speed) >> 12;
    p->vx = vx;
    p->vz = (gUnk_086149C4[*angle] * p->speed) >> 12;
    sprite->x += vx;
    height = target->y - sprite->y;
    if (height > 8)
        sprite->y += 8;
    else if (height < 8)
        sprite->y -= 8;
    else
        sprite->y = target->y;
    sprite->z += p->vz;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081D3750);
#endif
