// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: health and +0x2D8 pointer spills, mode lifetime,
// shared hit-flag expansion and temporary-register choices (see worker notes).
#include "scene_actor_reset.h"
#include "area_effect_link.h"
#include "render.h"
u8 SceneActorReaction_ApplyAreaDamage(void *);
void SceneActor_ClearReactionFlags(void *);
u8 SceneActorBehavior_DispatchFlaggedCategory(void *);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorBehavior_PlayReactionSound(void *);
void SceneActorReaction_RecordDefeat(void *);
void SceneActorTerrain_ClearCollisionTimers(void *);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void AnimData_SelectPaletteBank(void *, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 HasPendingHit(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2E6 == 0)
        return 0;
    return 1;
}
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        register u32 mask asm("r2") = *owner;
        if (!(mask & 0x20) && mask) {
            switch (LocalPlayer()) {
                case 0:
                    if (*owner != 1)
                        return 0;
                    break;
                case 1:
                    if (*owner != 2)
                        return 0;
                    break;
                case 2:
                    if (*owner != 4)
                        return 0;
                    break;
                case 3:
                    if (*owner != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}
static inline u8 IsDown(struct SceneActorRegistryResetView *actor, u32 flags)
{
    if (actor) {
        {
            u32 mask = 0x40;
            asm("" : "+r"(mask));
            if (mask & flags)
                return 1;
        }
        {
            u32 mask = 0x20000;
            if (actor->flags2D0 & mask)
                return 1;
        }
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline void UpdateHitFlags(struct SceneActorRegistryResetView *actor, u16 *flags2D4)
{
    u32 flags = *flags2D4;
    if (flags) {
        u32 down = IsDown(actor, flags);
        if (down) {
            u32 zero = 0;
            *flags2D4 = 0x40;
            actor->field2E6 = zero;
            {
                u16 *flags2D6 = &actor->flags2D6;
                s32 mask = ~0x980;
                *flags2D6 &= mask;
            }
            return;
        }
        {
            u32 mask = 0x803E;
            flags = *flags2D4;
            if (!(mask & flags))
                goto clear;
        }
        {
            u32 *flags2CC;
            {
                s32 mask = ~0x201;
                register u32 value asm("r0") = mask & flags;
                asm("" : "+r"(value));
                *flags2D4 = value;
            }
            flags2CC = &actor->flags2CC;
            {
                u32 mask = ~0x4000;
                *flags2CC &= mask;
            }
            {
                struct SceneActorReactionMotion *motion = (void *)&actor->fieldDC;
                s32 mask = ~0x10;
                motion->flags &= mask;
            }
            {
                u32 test = 0x20;
                if (!(*flags2D4 & test)) {
                    u32 mask = ~0x80;
                    *flags2CC &= mask;
                }
            }
            {
                u32 *flags2D0 = &actor->flags2D0;
                u32 mask = 0xFFEFFFFB;
                *flags2D0 &= mask;
            }
            {
                u32 bit = 0x400;
                actor->flags2D6 |= bit;
            }
            actor->field2E6 = down;
            {
                u32 bit = 0x2000;
                *flags2CC |= bit;
            }
            return;
        }
    }
clear: {
    u32 *flags2CC = &actor->flags2CC;
    u32 mask = ~0x2000;
    *flags2CC &= mask;
}
}

static inline void UpdateHitFlagsShared(struct SceneActorRegistryResetView *actor, u16 *flags2D4,
                                        u32 *flags2CC)
{
    u32 flags = *flags2D4;
    if (flags) {
        u32 down = IsDown(actor, flags);
        if (down) {
            u32 zero = 0;
            *flags2D4 = 0x40;
            actor->field2E6 = zero;
            {
                u16 *flags2D6 = &actor->flags2D6;
                s32 mask = ~0x980;
                *flags2D6 &= mask;
            }
            return;
        }
        {
            u32 mask = 0x803E;
            flags = *flags2D4;
            if (!(mask & flags))
                goto clearShared;
        }
        {
            {
                s32 mask = ~0x201;
                register u32 value asm("r0") = mask & flags;
                asm("" : "+r"(value));
                *flags2D4 = value;
            }
            {
                u32 mask = ~0x4000;
                *flags2CC &= mask;
            }
            {
                struct SceneActorReactionMotion *motion = (void *)&actor->fieldDC;
                s32 mask = ~0x10;
                motion->flags &= mask;
            }
            {
                u32 test = 0x20;
                if (!(*flags2D4 & test)) {
                    u32 mask = ~0x80;
                    *flags2CC &= mask;
                }
            }
            {
                u32 *flags2D0 = &actor->flags2D0;
                u32 mask = 0xFFEFFFFB;
                *flags2D0 &= mask;
            }
            {
                u32 bit = 0x400;
                actor->flags2D6 |= bit;
            }
            actor->field2E6 = down;
            {
                u32 bit = 0x2000;
                *flags2CC |= bit;
            }
            return;
        }
    }
clearShared: {
    u32 mask = ~0x2000;
    *flags2CC &= mask;
}
}

static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

u8 SceneActorReaction_ResolveDamage(struct SceneActorRegistryResetView *actor)
{
    u16 *flags2D4;
    s16 *health;
    register u32 mode asm("r10");
    u32 *flags2D8;
    u32 *flags2CC;
    u32 updated;
    register u32 four asm("r4");
    u32 bits;
    u32 k1;
    typeof(four) c1;
    if (!HasPendingHit(actor)) {
        if (SceneActorReaction_ApplyAreaDamage(actor)) {
            flags2D4 = &actor->flags2D4;
            UpdateHitFlags(actor, flags2D4);
            SceneActor_ClearReactionFlags(actor);
            return 1;
        }
        return 0;
    }
    if (SceneActorBehavior_DispatchFlaggedCategory(actor))
        return 1;
    SceneActor_ClearReactionFlags(actor);
    SceneActorMovement_ProjectProbe(actor, *((u16 *)(((u8 *)actor) + 0x174)),
                                    *((u16 *)(((u8 *)actor) + 0x174)));
    {
        u32 d = actor->reactionSoundVariant;
        health = &actor->health;
        *health -= d;
    }
    updated = (k1 = 0x20);
    flags2D4 = &actor->flags2D4;
    *flags2D4 = updated;
    updated |= *flags2D4;
    {
        u32 v = actor->field2E6;
        four = 4;
        c1 = four;
        bits = c1 & v;
        mode = 1;
        if (bits)
            mode = c1;
    }
    flags2D8 = &actor->field2D8;
    {
        u32 blocked = (*flags2D8) & 0x20000;
        flags2CC = &actor->flags2CC;
        if (blocked)
            goto done;
    }
    {
        if ((mode & four) && (!(updated & four))) {
            SceneActorBehavior_PlayReactionSound(actor);
            *((u16 *)(((u8 *)actor) + 0x30C)) = 12;
        }
        flags2CC = &actor->flags2CC;
        if (mode) {
            {
                u16 bank = *((u16 *)(((u8 *)actor) + 0x1E2));
                if (actor->drawableKind == 0)
                    AnimData_SelectPaletteBank(((u8 *)actor->drawable) + 0x2C, bank);
                else
                    *((u8 **)(((u8 *)actor->drawable) + 0x68)) = gPaletteBankData + (bank * k1);
            }
            {
                u32 eight = 8;
                actor->field1E8 = eight;
            }
            flags2CC = &actor->flags2CC;
            if ((mode & 4) && (actor->category == 0x19)) {
                {
                    u32 bit = 0x2000000;
                    *flags2CC |= bit;
                }
                {
                    u32 v = 0x41;
                    actor->field1EA = v;
                }
            }
            {
                u32 bit = 0x8000;
                *flags2CC |= bit;
            }
            if (ShouldShowDamage(actor)) {
                u32 zero;
                u32 buffer[2];
                register u32 one asm("r9");
                {
                    s32 h = *health;
                    s32 maxh = *((u16 *)(((u8 *)actor) + 0xB0));
                    s32 kind = *((s16 *)(((u8 *)actor) + 0xB2));
                    s32 param = *((s16 *)(((u8 *)actor) + 0x9A));
                    one = 1;
                    asm("" : "+r"(one));
                    SceneHud_SetTimedState(actor->id, h, maxh, kind, mode >> 2, param);
                }
                Fill(&zero, buffer);
                if (actor->reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                    DamageNumberDisplay_Show(&actor->colliderA, actor->reactionSoundVariant, 0,
                                             &actor->position, buffer, one);
            }
        }
    }
done: {
    u32 keep = 0x80;
    u32 value = *flags2CC;
    keep &= value;
    *flags2CC = (value & 0xFFFEBE3F) | keep;
}

    SceneActorReaction_RecordDefeat(actor);
    UpdateHitFlagsShared(actor, flags2D4, flags2CC);
    SceneActorTerrain_ClearCollisionTimers(actor);
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_ResolveDamage);
#endif
