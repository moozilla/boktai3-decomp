#include "paired_scene_actor.h"
#include "engine_globals.h"
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
            half->actionCounters[1] = clear;
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
            direction &= mask;
            offset->x = shift12(gUnk_086149C4[(direction + 64) & 255] * amplitude);
            offset->y = 0;
            offset->z = shift12(gUnk_086149C4[direction] * amplitude);
            {
                struct ContactPosition *dst = &half->sprite.position;
                struct ContactPosition *source = &jitter;
                struct ContactPosition *first = &jitter;
                dst->x = (u16)half->sprite.position.x + (u16)first->x;
                dst->y = (u16)dst->y + (u16)source->y;
                dst->z = (u16)dst->z + (u16)source->z;
            }
            {
                s32 *shake;
                shake = (s32 *)&half->shake;
                (*shake)--;
            }
        }
    }
    collider = &half->bodyCollider;
    {
        u32 fmask = 8;
        u32 *fp;
        u32 hidden;
        fp = &half->actionFlags;
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
        struct PairedHalfTarget *control;
        control = &half->target;
        {
            u32 mask = 1;
            control->flags |= mask;
        }
    }
    {
        u16 *timer;
        timer = &half->flashTimer;
        if (*timer) {
            (*timer)--;
            if (!*timer) {
                u16 **palp;
                void *src;
                palp = &half->palettes[0];
                src = *palp;
                CpuSet(src, half->graphics.palette, 0x04000008);
            }
        }
    }
    if (testFlagsP(flagsp, 0x80) || (half->sprite.flags & 1)) {
        ScaledShadow_Disable(half->shadow);
    } else {
        ScaledShadow_Enable(half->shadow);
    }
    {
        u8 *psp;
        psp = &half->requestedState;
        if (*psp == 0x16) {
            u8 *php;
            php = &half->finished;
            if (*php) {
                if (half->mode == 0) {
                    u32 *am;
                    u32 cur;
                    am = &owner->selectedMask;
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
