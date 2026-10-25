#include "global.h"

// confidence=low: contact response for a struct P / struct Q pair against actor R; sets timers, effect spawns and stats.
struct S0807E9D0Pair {
    u8 f0[6];
    u16 flags;
    u8 f8[2];
    u16 f0A;
    u8 f0C[0x28];
    u32 w34;
    u32 w38;
    u8 f3C[2];
    u16 f3C_u16;
    u16 f40;
    u8 f42;
    u8 f43;
    u16 f44;
};

u16 Div_u16_unused(void);

// confidence=low: contact response for a struct P / struct Q pair against actor R; sets timers, effect spawns and stats.

// confidence=low: link-state update: select the effect table entry from the flags, then either damage/contact or the stat update path.

// confidence=low: draft of the 1888-byte contact update.

// confidence=low: link-state update: select the effect table entry from the flags, then either damage/contact or the stat update path.

// confidence=low: draft of the 1888-byte contact update.
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void sub_08220204(void *, void *);
void Collision_CalculateDamageResponse(void *, void *);
void SceneActor_SetContactReactionDirection(void *, void *, void *);
void EffectGroup_SpawnPart(void *, s32, void *, void *, void *, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);

#define F8(p, off)    (*(u8 *)((u8 *)(p) + (off)))
#define F16(p, off)   (*(u16 *)((u8 *)(p) + (off)))
#define S16(p, off)   (*(s16 *)((u8 *)(p) + (off)))
#define F32(p, off)   (*(u32 *)((u8 *)(p) + (off)))
#define GU580(i)      (*(u32 *)(0x02000580 + 4 * (i)))
#define GU710()       (*(u32 *)0x02000710)
#define GUNK_03005308 (*(u32 *)0x03005308)
#define GUNK_030053F4 (*(u32 *)0x030053F4)

// confidence=low: link-state update: select the effect table entry from the flags, then either damage/contact or the stat update path.
static inline s32 S0807E9D0_SelectEntry(void *p, void *r)
{
    s32 flagged = 0;
    if (F16(p, 0x0A) & 1) {
        F32(r, 0xDC) = GU580(0) + 0x24;
        flagged = 1;
    } else if (F16(p, 0x0A) & 2) {
        F32(r, 0xDC) = GU580(1) + 0x24;
        flagged = 1;
    } else if (F16(p, 0x0A) & 4) {
        F32(r, 0xDC) = GU580(2) + 0x24;
        flagged = 1;
    } else if (F16(p, 0x0A) & 8) {
        F32(r, 0xDC) = GU580(3) + 0x24;
        flagged = 1;
    } else if (F16(p, 0x0A) & 0x10) {
        F32(r, 0xDC) = 0;
        flagged = 1;
    } else if (F16(p, 0x0A) & 0x20) {
        flagged = 1;
    }
    F32(r, 0x2E0) = F16(p, 0x0A);
    F32(r, 0x2D8) = F32(p, 0x34);
    F32(r, 0x2DC) = F32(p, 0x38);
    return flagged;
}

static inline void S0807E9D0_AimAtEntry(void *r)
{
    void *dc;
    if (F16(r, 0x2C0) & 2)
        F32(r, 0xDC) = GU580(0) + 0x24;
    else if (F32(r, 0xDC) == 0)
        return;
    dc = (void *)F32(r, 0xDC);
    {
        s32 dx = S16(dc, 0x0C) - S16(r, 0x5C);
        s32 dy = S16(dc, 0x10) - S16(r, 0x60);
        F8(r, 0xE9) = Math_Atan2(dx, dy);
        F32(r, 0xF0) = dx * dx + dy * dy;
    }
}

static inline void S0807E9D0_Widen(void *r)
{
    F16(r, 0x418) += 0x12C;
    if (F16(r, 0x418) > F16(r, 0x416))
        F16(r, 0x418) = F16(r, 0x416);
}

#ifdef NONMATCHING
// fuzzy: 35.9%
// scope shape 465 (differences once low registers are set aside)
// confidence=low: draft of the 1888-byte contact update.
void sub_0807E9D0(void *p, void *q, void *r)
{
    s32 i;
    s32 flagged;
    u32 pos[2];
    u32 part[2];
    u32 vec[2];
    u32 idx;

    if (F16(r, 0x2D4) & 0x40)
        return;
    if (F32(r, 0x2D0) & 0x20000)
        return;
    if (S16(r, 0x9C) <= 0)
        return;

    if (F16(r, 0x178) != 0) {
        if (F16(r, 0x2E6) == 0)
            return;
        F16(q, 0x3E) = 0;
        F8(q, 0x43) = 0;
        return;
    }
    if (F16(r, 0x13A) & 4)
        return;
    if (F16(p, 6) & 1)
        goto bigpath;

    if (!(F32(p, 0x34) & 0x20000))
        return;
    if (F16(r, 0x2D4) & 0xA007)
        return;
    if (F16(r, 0x5C8) != 0)
        return;
    F16(r, 0x5C8) = 0x3C;
    if (F32(r, 0x2D0) & 0x8000000) {
        if ((F16(q, 6) & 0x100) && (F16(p, 6) & 0x100)) {
            s32 d = F8(q, 0x42) - F8(p, 0x42);
            if (d < 0)
                d = -d;
            if (d <= 0x20) {
                F16(r, 0x2B8) = F16(p, 0x3C);
                F16(r, 0x2E6) = 1;
                F16(r, 0x5CE) = Div(F16(r, 0x2B8), 3);
                F16(r, 0x2D4) |= 0x2000;
                return;
            }
        }
    }
    F16(q, 0x3E) = 0;
    F16(r, 0x2E4) = 0;
    flagged = S0807E9D0_SelectEntry(p, r);
    if (flagged)
        S0807E9D0_AimAtEntry(r);
    if (F16(r, 0x2D4) & 0x80)
        S0807E9D0_Widen(r);
    F16(r, 0x2E6) = 1;
    return;

bigpath:
    if (GUNK_030053F4 & 0x4800)
        sub_08220204(p, q);
    else
        Collision_CalculateDamageResponse(p, q);
    F16(q, 0x44) = F16(p, 0x44);
    {
        u16 t = F8(q, 0x43);
        F16(r, 0x2E6) = t;
    }
    if (F32(p, 0x38) & 0x40000) {
        s32 v = F16(p, 0x40) - F8(r, 0x9F);
        F16(q, 0x40) = v < 0 ? 0 : v;
    }
    if (F16(r, 0x2D4) & 6) {
        F16(q, 0x3E) = F16(q, 0x3E) << 1;
        S16(q, 0x40) = S16(q, 0x40) / 2;
    }
    SceneActor_SetContactReactionDirection(r, p, q);
    flagged = S0807E9D0_SelectEntry(p, r);
    if (flagged)
        S0807E9D0_AimAtEntry(r);
    if (F16(r, 0x2E6) != 0 || (F32(p, 0x38) & 0x40)) {
        if (F16(r, 0x2D4) & 0x80)
            S0807E9D0_Widen(r);
    }
    if ((F32((void *)GU710(), 0x868) & 0x800) || (F32(p, 0x38) & 0x40)) {
        F16(q, 0x3E) = 0;
        F16(r, 0x2E4) = 0;
        F16(r, 0x2E6) = 1;
    }
    if (F16(r, 0x2D4) & 0x2000) {
        for (i = 2; i >= 0; i--) {
            u16 *tab = (u16 *)0x0203B400;
            idx = GUNK_03005308;
            pos[0] = (pos[0] & 0xFFFF0000) | (u16)((tab[(idx + 1) & 0x3FF] & 0xF) - 7);
            pos[0] = (pos[0] & 0xFFFF) |
                     (u32)((((tab[(idx + 2) & 0x3FF] & 0x1F) - 0x10) & 0xFFFF) << 16);
            GUNK_03005308 = (idx + 3) & 0x3FF;
            pos[1] = (pos[1] & 0xFFFF0000) | (u16)((tab[(idx + 3) & 0x3FF] & 0xF) - 7);
            part[0] = 0xA0005;
            part[1] = (part[1] & 0xFFFF0000) | 5;
            {
                u16 *ent = (u16 *)F32(r, 0x484);
                vec[0] = (vec[0] & 0xFFFF0000) | ent[0];
                vec[0] = (vec[0] & 0xFFFF) | ((u32)(u16)(ent[1] + 0x80) << 16);
                vec[1] = (vec[1] & 0xFFFF0000) | ent[2];
            }
            EffectGroup_SpawnPart(r + 0x47C, 3, vec, pos, part, 0x28, 0x28);
        }
        F16(r, 0xAA) = 0;
        F16(r, 0x2B8) = 0;
    }
    F16(r, 0x2E4) = F16(q, 0x3E);
    PlayerStats_AddContactHundredths(p, q, F8(r, 0xA0));
    F32(r, 0x2CC) |= 0x2000;
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807E9D0);
#endif
