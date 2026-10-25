#include "global.h"

// confidence=low: per-frame actor state machine; counts down timers, then picks an aim, attack or retreat behavior from the offset to the player.
typedef struct Actor08187F44 {
    u8 pad0[0x48];
    s16 x;
    s16 z;
    s16 y;
    u8 pad4e[0x292 - 0x4e];
    u16 bits292;
    u8 pad294[0xb32 - 0x294];
    u16 bitsB32;
    u8 padb34[0xb39 - 0xb34];
    u8 angle;
    u8 padb3a[0xb3c - 0xb3a];
    u8 behavior;
    u8 pre;
    u8 padb3e[0x1120 - 0xb3e];
    u8 state;
    u8 flag1121;
    s16 timer1;
    u16 pad1124;
    s16 timer2;
    u16 timer3;
} Actor08187F44;

typedef struct Player08187F44 {
    u8 pad0[0x30];
    s16 x;
    s16 z;
    s16 y;
} Player08187F44;

void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Armor_IsSequenceIncomplete(void *);
s32 Math_Atan2(s32, s32);

static inline void s08187F44_Pick(Actor08187F44 *a)
{
    u32 k;
    k = (*(u32 *)0x03005308 + 1) & 0x3ff;
    *(u32 *)0x03005308 = k;
    if (((u16 *)0x0203B400)[k] & 4)
        ActorController_SetBehavior(a, 0x24);
    else
        ActorController_SetBehavior(a, 0x18);
    a->timer1 = 0xb4;
}

#ifdef NONMATCHING
// fuzzy: 54.7%
// scope shape 174 (differences once low registers are set aside)
void sub_08187F44(Actor08187F44 *a)
{
    Player08187F44 *pl;
    s32 dx, dy, dz, cdx, cdy, adx, ady, adz, dist, ang;
    u16 t;

    t = a->timer1;
    if ((s16)t > 0) {
        a->timer1 = t - 1;
        if (a->flag1121)
            a->timer1 = t - 2;
    }
    t = a->timer2;
    if ((s16)t > 0) {
        a->timer2 = t - 1;
        if (a->flag1121)
            a->timer2 = t - 2;
    }
    if (a->timer2 > 0 || (a->bitsB32 & 1))
        a->bits292 |= 4;
    else
        a->bits292 &= ~4;

    a->timer3 = a->timer3 - 1;
    if ((s16)a->timer3 <= 0 && !(a->bitsB32 & 1)) {
        a->flag1121 = (a->flag1121 == 0);
        a->timer3 = 0x12c;
    }

    if (!(a->pre == 0xff || a->behavior == 0x13 || a->behavior == 0x21 || a->behavior == 0x20 ||
          a->behavior == 0x22))
        return;

    pl = *(Player08187F44 **)0x02000710;
    dx = pl->x - a->x;
    dz = pl->z - a->z;
    dy = pl->y - a->y;
    cdx = (dx + 0x80) >> 8;
    cdy = (dy + 0x80) >> 8;
    adx = cdx < 0 ? -cdx : cdx;
    adz = dz < 0 ? -dz : dz;
    ady = cdy < 0 ? -cdy : cdy;
    dist = adx + ady;

    if (a->bitsB32 & 1) {
        if (a->state == 0) {
            if (dist <= 3) {
                ActorController_SetBehavior(a, 0x20);
                a->state = 1;
                a->timer1 = 0xb4;
            } else {
                ActorController_SetBehaviorIfFinished(a, 0x13);
            }
        } else if (a->state == 1) {
            if (a->behavior == 3) {
                ActorController_SetBehavior(a, 0x1e);
                return;
            }
            if (dist > 4) {
                ActorController_SetBehavior(a, 0x13);
                a->state = 0;
                return;
            }
            ang = (Math_Atan2(dx, dy) + 0x20) & 0xc0;
            if (a->angle == ang && dist <= 1) {
                ActorController_SetBehavior(a, 3);
                return;
            }
            if (a->timer1 > 0) {
                ActorController_SetBehaviorIfFinished(a, 0x20);
                return;
            }
            if (cdx == 0) {
                if (cdy < 0 ? a->angle == 0xc0 : a->angle == 0x40) {
                    s08187F44_Pick(a);
                    return;
                }
                ActorController_SetBehaviorIfFinished(a, 0x20);
                return;
            }
            if (cdy != 0) {
                ActorController_SetBehaviorIfFinished(a, 0x20);
                return;
            }
            if (cdx < 0 ? a->angle == 0x80 : a->angle == 0) {
                s08187F44_Pick(a);
                return;
            }
            ActorController_SetBehaviorIfFinished(a, 0x20);
        } else if (a->state == 2) {
            if (a->behavior == 0xf) {
                ActorController_SetBehavior(a, 3);
                return;
            }
            ActorController_SetBehavior(a, 0x20);
            a->state = 1;
            a->timer1 = 0xb4;
        } else if (a->state == 3) {
            s32 b = a->behavior;
            if ((u8)(b - 10) > 1 && b != 0xd)
                ActorController_SetBehavior(a, 0xa);
            if (a->timer1 > 0)
                return;
            if (cdx == 0) {
                if ((cdy < 0 ? a->angle == 0xc0 : a->angle == 0x40) && adz <= 0x7f && ady <= 4) {
                    ActorController_SetBehavior(a, 0x24);
                    a->state = 0;
                    return;
                }
            } else if (cdy == 0 && (cdx < 0 ? a->angle == 0x80 : a->angle == 0) && adz <= 0x7f &&
                       adx <= 4) {
                ActorController_SetBehavior(a, 0x24);
                a->state = 0;
                return;
            }
            ActorController_SetBehavior(a, 0x13);
            a->state = 0;
        }
    } else {
        if (a->state == 0) {
            if (dist <= 3) {
                if (a->flag1121)
                    ActorController_SetBehavior(a, 0x22);
                else
                    ActorController_SetBehavior(a, 0x20);
                a->state = 1;
                a->timer1 = 0xb4;
            } else if (Armor_IsSequenceIncomplete((u8 *)a + 0xb58)) {
                ActorController_SetBehavior(a, 8);
            } else if (a->flag1121) {
                ActorController_SetBehavior(a, 0x21);
            } else {
                ActorController_SetBehavior(a, 0x13);
            }
        } else if (a->state == 1) {
            if (a->behavior == 3) {
                ActorController_SetBehavior(a, 0x1e);
                return;
            }
            if (dist > 4) {
                if (a->flag1121)
                    ActorController_SetBehavior(a, 0x21);
                else
                    ActorController_SetBehavior(a, 0x13);
                a->state = 0;
                return;
            }
            ang = (Math_Atan2(dx, dy) + 0x20) & 0xc0;
            if (a->angle == ang && dist <= 1) {
                ActorController_SetBehavior(a, 3);
                return;
            }
            if (a->timer1 > 0) {
                if (a->flag1121)
                    ActorController_SetBehavior(a, 0x22);
                else
                    ActorController_SetBehavior(a, 0x20);
                return;
            }
            if (cdx == 0) {
                if (cdy < 0 ? a->angle == 0xc0 : a->angle == 0x40) {
                    s08187F44_Pick(a);
                    return;
                }
            } else if (cdy == 0 && (cdx < 0 ? a->angle == 0x80 : a->angle == 0)) {
                s08187F44_Pick(a);
                return;
            }
            if (a->flag1121)
                ActorController_SetBehavior(a, 0x22);
            else
                ActorController_SetBehavior(a, 0x20);
        } else if (a->state == 2) {
            if (a->behavior == 0xf) {
                ActorController_SetBehavior(a, 3);
                return;
            }
            if (a->flag1121)
                ActorController_SetBehavior(a, 0x22);
            else
                ActorController_SetBehavior(a, 0x20);
            a->state = 1;
            a->timer1 = 0xb4;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08187F44);
#endif
