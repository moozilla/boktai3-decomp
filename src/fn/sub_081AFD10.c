#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define PairedSceneActor_UpdateHalfPresentation sub_081AFD10
#ifdef NONMATCHING
// fuzzy: 85.8%
// confidence=low: updates each half's visible position, colliders and completion transition.
struct PairPosition {
    s16 x, y, z, pad;
};
struct PairCollider {
    u8 pad0[6];
    s16 flags : 16;
    u8 pad8[0x54 - 8];
};
struct PairShadowControl {
    u32 unused;
    u32 flags;
};
struct PairHalf {
    u32 displayFlags;
    u8 pad4[0x1C - 4];
    struct PairPosition position;
    u8 pad24[0x38 - 0x24];
    void *palette;
    u8 pad3C[0x90 - 0x3C];
    struct PairPosition *origin;
    u8 pad94[8];
    struct PairPosition offset;
    u8 padA4[0xB4 - 0xA4];
    struct PairCollider body;
    struct PairCollider attack;
    u32 mode;
    u32 flags;
    u32 reset164, reset168;
    u8 state, previousState, entered, pad16F, phase;
    u8 pad171[3];
    s16 duration;
    u8 pad176[2];
    u32 timer;
    u8 pad17C[8];
    u16 flashTimer;
    u8 hiddenTimer;
    u8 pad187[9];
    void *savedPalette;
    u8 pad194[8];
    s32 shake;
    u8 shadow[0x6C];
    struct PairShadowControl shadowControl;
    u8 pad214[8];
};
struct PairOwner {
    u8 pad0[0x5B4];
    u32 activeMask;
};
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_Enable(void *);
static inline s32 randomSample(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    register s32 result asm("r0");
    gUnk_03005308 = index;
    result = *(u16 *)(RANDOM_TABLE_ADDR + index * 2);
    asm("" : "+r"(result));
    return result;
}
static inline s32 shift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u32 testFlags(struct PairHalf *half, u32 mask)
{
    return half->flags & mask;
}
static inline void setFlags(struct PairHalf *half, u32 mask)
{
    half->flags |= mask;
}
static inline void setReady(struct PairHalf *half)
{
    u32 next = 2;
    u32 mask = 0x100;
    u32 clear = half->flags & mask;
    if (!clear) {
        u32 previous = (u8)(half->previousState - 0x19);
        u8 *phase = &half->phase;
        if (previous > 1 || *phase) {
            half->previousState = next;
            half->state = next;
            half->entered = 1;
            *phase = clear;
            half->timer = clear;
            half->reset164 = clear;
            half->reset168 = clear;
        }
    }
}
void PairedSceneActor_UpdateHalfPresentation(struct PairOwner *owner, struct PairHalf *half)
{
    struct PairPosition jitter;
    struct PairCollider *collider;
    {
        struct PairPosition *dst = &half->position;
        register struct PairPosition *offset asm("r1");
        register struct PairPosition *base asm("r2") = half->origin;
        offset = &half->offset;
        asm("" : : "r"(offset), "r"(base));
        dst->x = offset->x + base->x;
        dst->y = offset->y + base->y;
        dst->z = offset->z + base->z;
    }
    half->position.y += 256;
    if (half->hiddenTimer) {
        setFlags(half, 4);
        half->hiddenTimer--;
    }
    {
        register s32 amplitude asm("r6") = half->shake;
        asm("" : : "r"(amplitude));
        if (amplitude) {
            register s32 direction asm("r3") = randomSample() >> 4;
            register struct PairPosition *offset asm("r4") = &jitter;
            register const s16 *sine asm("r2");
            u32 mask = 255;
            asm("" : : "r"(offset));
            direction &= mask;
            sine = gUnk_086149C4;
            asm("" : : "r"(sine), "r"(direction));
            offset->x = shift12(sine[(direction + 64) & 255] * amplitude);
            offset->y = 0;
            offset->z = shift12(sine[direction] * amplitude);
            {
                struct PairPosition *dst = &half->position;
                struct PairPosition *source = &jitter;
                struct PairPosition *first = &jitter;
                register u32 coordinate asm("r0");
                register u32 deltaX asm("r3");
                register u32 deltaY asm("r4");
                register u32 deltaZ asm("r6");
                asm("" : "+r"(first));
                coordinate = (u16)first->x;
                deltaX = (u16)half->position.x;
                asm("" : : "r"(coordinate), "r"(deltaX));
                dst->x = coordinate + deltaX;
                coordinate = (u16)source->y;
                deltaY = (u16)dst->y;
                asm("" : : "r"(coordinate), "r"(deltaY));
                dst->y = coordinate + deltaY;
                coordinate = (u16)source->z;
                deltaZ = (u16)dst->z;
                asm("" : : "r"(coordinate), "r"(deltaZ));
                dst->z = coordinate + deltaZ;
            }
            half->shake--;
        }
    }
    collider = &half->body;
    {
        u32 hidden = testFlags(half, 8);
        if (!hidden) {
            Collider_SetPosition(collider, &half->position, 0);
            Collider_Refresh(collider);
        }
    }
    collider = &half->attack;
    if (testFlags(half, 4)) {
        collider->flags |= 4;
    } else {
        s32 clearMask = ~4;
        collider->flags &= clearMask;
        Collider_SetPosition(collider, &half->position, 0);
    }
    if (testFlags(half, 0x20)) {
        register struct PairShadowControl *control asm("r2") = &half->shadowControl;
        s32 mask = ~1;
        asm("" : "+r"(control));
        control->flags &= mask;
    } else {
        register struct PairShadowControl *control asm("r2") = &half->shadowControl;
        u32 mask = 1;
        asm("" : "+r"(control));
        control->flags |= mask;
    }
    if (half->flashTimer) {
        half->flashTimer--;
        if (!half->flashTimer)
            CpuSet(half->savedPalette, half->palette, 0x04000008);
    }
    if (testFlags(half, 0x80) || (half->displayFlags & 1))
        ScaledShadow_Disable(half->shadow);
    else
        ScaledShadow_Enable(half->shadow);
    if (half->previousState == 0x16 && half->phase) {
        if (half->mode == 0)
            owner->activeMask &= ~1;
        else
            owner->activeMask &= ~2;
        setReady(half);
    }
    half->state = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081AFD10);
#endif
