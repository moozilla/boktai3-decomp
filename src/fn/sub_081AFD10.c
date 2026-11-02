#include "paired_scene_actor.h"
#include "engine_globals.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: updates each half's visible position, colliders and completion transition.
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
static inline u32 testFlags(struct PairedSceneHalf *half, u32 mask)
{
    return half->actionFlags & mask;
}
static inline u32 testFlagsP(u32 *fp, u32 mask)
{
    return *fp & mask;
}
static inline void setFlags(struct PairedSceneHalf *half, u32 mask)
{
    half->actionFlags |= mask;
}
static inline void setReady(struct PairedSceneHalf *half, u32 *fl)
{
    u32 next = 2;
    u32 mask = 0x100;
    u32 clear = *fl & mask;
    if (!clear) {
        u32 previous = (u8)(half->requestedState - 0x19);
        u8 *phase = &half->finished;
        if (previous > 1 || *phase) {
            half->requestedState = next;
            half->state = next;
            half->entered = 1;
            *phase = clear;
            half->stateTimer = clear;
            half->actionCounters[0] = clear;
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
void PairedSceneActor_UpdateHalfPresentation(struct VanargandAttackPair *owner,
                                             struct PairedSceneHalf *half)
{
    struct ContactPosition jitter;
    struct ContactCollider *collider;
    u32 *flagsp;
    {
        struct ContactPosition *dst = &half->sprite.position;
        struct ContactPosition *offset;
        struct ContactPosition *base = half->origin;
        offset = &half->offset;
        dst->x = base->x + offset->x;
        dst->y = base->y + offset->y;
        dst->z = base->z + offset->z;
    }
    half->sprite.position.y += 256;
    if (half->hiddenTimer) {
        setFlags(half, 4);
        half->hiddenTimer--;
    }
    {
        s32 amplitude = (s32)half->shake;
        if (amplitude) {
            s32 direction = randomSample() >> 4;
            u32 mask = 255;
            struct ContactPosition *offset = &jitter;
            register const s16 *sine asm("r2");
            direction &= mask;
            sine = gUnk_086149C4;
            offset->x = shift12(sine[(direction + 64) & 255] * amplitude);
            offset->y = 0;
            offset->z = shift12(sine[direction] * amplitude);
            {
                struct ContactPosition *dst = &half->sprite.position;
                struct ContactPosition *source = &jitter;
                struct ContactPosition *first = &jitter;
                register u32 coordinate asm("r0");
                register u32 deltaX asm("r3");
                register u32 deltaY asm("r4");
                register u32 deltaZ asm("r6");
                coordinate = (u16)first->x;
                deltaX = (u16)half->sprite.position.x;
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
    collider = &half->bodyCollider;
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
            Collider_SetPosition(collider, &half->sprite.position, 0);
            Collider_Refresh(collider);
        }
    }
    collider = &half->damageCollider;
    if (testFlagsP(flagsp, 4)) {
        collider->flags |= 4;
    } else {
        s32 clearMask = ~4;
        collider->flags &= clearMask;
        Collider_SetPosition(collider, &half->sprite.position, 0);
    }
    if (testFlagsP(flagsp, 0x20)) {
        struct PairedHalfTarget *control = &half->target;
        s32 mask = ~1;
        control->flags &= mask;
    } else {
        register u32 offset asm("r3") = 0x20C;
        register struct PairedHalfTarget *control asm("r2");
        u32 mask = 1;
        asm("" : "+r"(offset));
        control = (struct PairedHalfTarget *)((u8 *)half + offset);
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
                CpuSet(src, half->graphics.palette, 0x04000008);
            }
        }
    }
    if (testFlagsP(flagsp, 0x80) || (half->sprite.flags & 1)) {
        ScaledShadow_Disable(half->shadow);
    } else {
        register u32 offset asm("r3") = 0x1A0;
        asm("" : "+r"(offset));
        ScaledShadow_Enable((u8 *)half + offset);
    }
    {

        u8 *psp;
        psp = &half->requestedState;
        if (*psp == 0x16) {

            u8 *php;
            php = &half->finished;
            if (*php) {

                if (half->mode == 0) {
                    register u32 aoff asm("r3") = 0x5B4;
                    register u32 *am asm("r0");
                    u32 cur;
                    asm("" : "+r"(aoff));
                    am = (u32 *)((u8 *)owner + aoff);
                    cur = *am;
                    *am = cur & ~1;
                } else {
                    owner->selectedMask &= ~2;
                }
                setReady(half, flagsp);
            }
        }
    }
    half->state = 0;
}
