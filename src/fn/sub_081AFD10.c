#include "global.h"
#include "engine_globals.h"
// confidence=low: maintains paired-half presentation, colliders and completion flags.
// MATCHING_CONCESSION: register-pinning
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
extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_Enable(void *);
static inline s32 randomSample(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    s32 result;
    gRandomIndex = index;
    result = *(u16 *)(RANDOM_TABLE_ADDR + index * 2);
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
static inline u32 testFlagsP(u32 *fp, u32 mask)
{
    return *fp & mask;
}
static inline void setFlags(struct PairHalf *half, u32 mask)
{
    half->flags |= mask;
}
static inline void setReady(struct PairHalf *half, u32 *fl)
{
    u32 next = 2;
    u32 mask = 0x100;
    u32 clear = *fl & mask;
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
            {
                register u32 offset asm("r3") = 0x168;
                asm("" : "+r"(offset));
                {
                    register u32 *dst asm("r0") = (u32 *)((u8 *)half + offset);
                    *dst = clear;
                }
            }
        }
    }
}
void PairedSceneActor_UpdateHalfPresentation(struct PairOwner *owner, struct PairHalf *half)
{
    struct PairPosition jitter;
    struct PairCollider *collider;
    u32 *flagsp;
    {
        struct PairPosition *dst = &half->position;
        struct PairPosition *offset;
        struct PairPosition *base = half->origin;
        offset = &half->offset;
        dst->x = base->x + offset->x;
        dst->y = base->y + offset->y;
        dst->z = base->z + offset->z;
    }
    half->position.y += 256;
    if (half->hiddenTimer) {
        setFlags(half, 4);
        half->hiddenTimer--;
    }
    {
        s32 amplitude = half->shake;
        if (amplitude) {
            s32 direction = randomSample() >> 4;
            u32 mask = 255;
            struct PairPosition *offset = &jitter;
            register const s16 *sine asm("r2");
            direction &= mask;
            sine = gUnk_086149C4;
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
                coordinate = (u16)first->x;
                deltaX = (u16)half->position.x;
                dst->x = coordinate + deltaX;
                coordinate = (u16)source->y;
                deltaY = (u16)dst->y;
                dst->y = coordinate + deltaY;
                coordinate = (u16)source->z;
                deltaZ = (u16)dst->z;
                dst->z = coordinate + deltaZ;
            }
            {
                register u32 offset asm("r0") = 0x19C;
                register s32 *shake asm("r1");
                asm("" : "+r"(offset));
                shake = (s32 *)((u8 *)half + offset);
                (*shake)--;
            }
        }
    }
    collider = &half->body;
    {
        u32 fmask = 8;
        register u32 foff asm("r3") = 0x160;
        register u32 *fp asm("r1");
        u32 hidden;
        asm("" : "+r"(foff));
        fp = (u32 *)((u8 *)half + foff);
        hidden = *fp & fmask;
        flagsp = fp;
        if (!hidden) {
            Collider_SetPosition(collider, &half->position, 0);
            Collider_Refresh(collider);
        }
    }
    collider = &half->attack;
    if (testFlagsP(flagsp, 4)) {
        collider->flags |= 4;
    } else {
        s32 clearMask = ~4;
        collider->flags &= clearMask;
        Collider_SetPosition(collider, &half->position, 0);
    }
    if (testFlagsP(flagsp, 0x20)) {
        struct PairShadowControl *control = &half->shadowControl;
        s32 mask = ~1;
        control->flags &= mask;
    } else {
        register u32 offset asm("r3") = 0x20C;
        register struct PairShadowControl *control asm("r2");
        u32 mask = 1;
        asm("" : "+r"(offset));
        control = (struct PairShadowControl *)((u8 *)half + offset);
        asm("" : "+r"(mask));
        control->flags |= mask;
    }
    {
        register u32 offset asm("r4") = 0x184;
        u16 *timer;
        asm("" : "+r"(offset));
        timer = (u16 *)((u8 *)half + offset);
        if (*timer) {
            (*timer)--;
            if (!*timer) {
                register u32 paletteOffset asm("r1") = 0x190;
                register void **palp asm("r0");
                void *src;
                asm("" : "+r"(paletteOffset));
                palp = (void **)((u8 *)half + paletteOffset);
                src = *palp;
                CpuSet(src, half->palette, 0x04000008);
            }
        }
    }
    if (testFlagsP(flagsp, 0x80) || (half->displayFlags & 1)) {
        u32 offset = 0x1A0;
        ScaledShadow_Disable((u8 *)half + offset);
    } else {
        register u32 offset asm("r3") = 0x1A0;
        asm("" : "+r"(offset));
        ScaledShadow_Enable((u8 *)half + offset);
    }
    {
        u32 psoff = 0x16D;
        u8 *psp;
        psp = (u8 *)half + psoff;
        if (*psp == 0x16) {
            u32 phoff = 0x170;
            u8 *php;
            php = (u8 *)half + phoff;
            if (*php) {
                u32 moff = 0x15C;
                u32 *mp;
                mp = (u32 *)((u8 *)half + moff);
                if (*mp == 0) {
                    register u32 aoff asm("r3") = 0x5B4;
                    register u32 *am asm("r0");
                    u32 cur;
                    asm("" : "+r"(aoff));
                    am = (u32 *)((u8 *)owner + aoff);
                    cur = *am;
                    *am = cur & ~1;
                } else {
                    u32 aoff = 0x5B4;
                    u32 *am;
                    u32 cur;
                    am = (u32 *)((u8 *)owner + aoff);
                    cur = *am;
                    *am = cur & ~2;
                }
                setReady(half, flagsp);
            }
        }
    }
    half->state = 0;
}
