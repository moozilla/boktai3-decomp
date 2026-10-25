#include "global.h"

// Inlined Select helper (same shape as the Ghost_TurnTowardHeading unit's Select).

// confidence=low: re-aims the turn toward the desired facing and picks the animation for each octant.
void Anim_SetSequence(void *, void *, u16, u8, u8);
void Anim_SetKeyframes(void *, void *, u16, u8, u16);
void Anim_SetDurationScale(void *, u16);
void Anim_SetFrameDurationScale(void *, u16);
u16 Route_GetCurrentPointFacing(void *);
void SceneActorAnimation_SelectDirectionalEntry(u8 *, u32);

// Inlined Select helper (same shape as the Ghost_TurnTowardHeading unit's Select).
static inline void Sub5930_Select(u8 *a, u32 selection, u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    u8 *drawable = *(u8 **)(a + 4);

    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!a[8]) {
        a[0x2B7] = selection;
        Anim_SetSequence(drawable + 0x48, *(void **)(drawable + 0x58), (u16)selection, (u8)variant,
                         (u8)flags);
        restart = a + 0x2B3;
    } else {
        u8 *tile = drawable + 0x20;
        u8 *savedSelection;
        selection += variant;
        savedSelection = a + 0x2B7;
        restart = a + 0x2B3;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (*(u32 *)(tile + 8) & 4))
                return;
            if (flags == 0 && !(*(u32 *)(tile + 8) & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mode = (flags & 4) ? 6 : 4;
            Anim_SetKeyframes(tile, drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                *(u32 *)(tile + 8) |= 4;
            else
                *(u32 *)(tile + 8) &= ~4;
        }
    }
    *restart = 0;
}

static inline void Sub5930_Control(u8 *a, void *cb, u32 kind)
{
    a[0x2B2] = 1;
    a[0x2B0] = 0;
    a[0x2B5] = kind;
    *(void **)(a + 0x21C) = cb;
    *(u16 *)(a + 0x2C8) = 0;
    *(u32 *)(a + 0x114) &= ~1;
    *(u32 *)(a + 0x104) = 0;
}

static inline u32 Sub5930_Take(u8 *q)
{
    if (*q) {
        *q = 0;
        return 1;
    }
    return 0;
}

#ifdef NONMATCHING
// fuzzy: 29.5%
// scope shape 567 (differences once low registers are set aside)
// confidence=low: re-aims the turn toward the desired facing and picks the animation for each octant.
void sub_080C5930(u8 *p)
{
    s32 quadrant;
    s32 desired;
    u32 d;

    if (!Sub5930_Take(p + 0x2B2))
        goto tail;
    {
        u8 *heading = p + 0x59;
        *heading = ((((*heading + 0x60) >> 6) & 3) << 6) - 0x40;
        quadrant = ((*heading + 0x60) >> 6) & 3;
    }
    if (p[0x2AA] != 6 && !(*(u16 *)(p + 0x2D4) & 0x480))
        p[0x3CD] = ((u16)Route_GetCurrentPointFacing(*(void **)(p + 0x78)) + 0x70) >> 5 & 7;
    desired = (s8)p[0x3CD];
    if (quadrant == desired / 2) {
        s32 setup = *(u16 *)(p + 0x2D4) & 0x400;

        p[0x2B0] = 1;
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        if (setup) {
            p[0x2A8] = 3;
            p[0x2A9] = 1;
            p[0x2AA] = 0;
            *(u32 *)(p + 0x2AC) = 0;
            p[0x2B1] = 1;
            Sub5930_Control(p, *(void **)(p + 0x5F0), 0x1B);
        }
        return;
    }
    d = p[0x3CD];
    switch (quadrant) {
        case 0:
            if (d - 2 <= 3) {
                Sub5930_Select(p, 2, 1, 0);
                *(u32 *)(p + 0x2CC) |= 0x10000;
            } else {
                Sub5930_Select(p, 2, 0, 4);
                *(u32 *)(p + 0x2CC) &= ~0x10000;
            }
            break;
        case 1:
            if (d - 4 <= 3) {
                Sub5930_Select(p, 2, 2, 0);
                *(u32 *)(p + 0x2CC) |= 0x10000;
            } else {
                Sub5930_Select(p, 2, 1, 4);
                *(u32 *)(p + 0x2CC) &= ~0x10000;
            }
            break;
        case 2:
            if (d - 1 <= 4) {
                Sub5930_Select(p, 2, 2, 4);
                *(u32 *)(p + 0x2CC) &= ~0x10000;
            } else {
                Sub5930_Select(p, 2, 1, 5);
                *(u32 *)(p + 0x2CC) |= 0x10000;
            }
            break;
        case 3:
            if (d > 3) {
                Sub5930_Select(p, 2, 1, 1);
                *(u32 *)(p + 0x2CC) &= ~0x10000;
            } else {
                Sub5930_Select(p, 2, 0, 0);
                *(u32 *)(p + 0x2CC) |= 0x10000;
            }
            break;
        default:
            break;
    }
    p[0x2B3] = 0;
tail:
    if (*(u16 *)(*(u8 **)(p + 4) + 0x48 + 0xE) == 0) {
        s32 target = (((p[0x3CD] + 5) & 7) << 5);
        s32 diff = p[0x59] - target;
        if (diff < 0)
            diff = -diff;
        if (diff <= 0x16)
            p[0x59] = target;
        else if (*(u32 *)(p + 0x2CC) & 0x10000)
            p[0x59] = p[0x59] + 0x16;
        else
            p[0x59] = p[0x59] - 0x16;
    }
    {
        u16 *scale = (u16 *)(p + 0x2C2);
        if (*(u16 *)(p + 0x2D4) & 0xA02F) {
            *scale = 64;
        } else if (*scale == 0) {
            *scale = 64;
        }
        if (!p[8])
            Anim_SetDurationScale(*(u8 **)(p + 4) + 0x48, *scale);
        else
            Anim_SetFrameDurationScale(*(u8 **)(p + 4) + 0x20, *scale);
    }
    if (!p[0x2B3])
        return;
    {
        s32 cur = (((s32)p[0x59] + 0x70) >> 5) & 7;
        s32 diff = (s32)p[0x3CD] - cur;
        if (diff < 0)
            diff = -diff;
        if (diff > 0) {
            Sub5930_Control(p, *(void **)(p + 0x224), 2);
            return;
        }
    }
    p[0x2B0] = 1;
    p[0x59] = ((((p[0x59] + 0x60) >> 6) & 3) << 6) - 0x40;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
    if (*(u16 *)(p + 0x2D4) & 0x400) {
        p[0x2A8] = 3;
        p[0x2A9] = 1;
        p[0x2AA] = 0;
        *(u32 *)(p + 0x2AC) = 0;
        p[0x2B1] = 1;
        Sub5930_Control(p, *(void **)(p + 0x5F0), 0x1B);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C5930);
#endif
