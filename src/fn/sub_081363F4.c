// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "area_effect_link.h"
#include "render.h"
void SceneActor_ClearReactionFlags(void *);
u8 SceneActorBehavior_DispatchFlaggedCategory(void *);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorBehavior_PlayReactionSound(void *);
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
static inline s32 LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        u32 mask = *owner;
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
                u32 value = mask & flags;
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

// confidence=low: expanded scene-actor hit processing with route and collider cleanup.
#define SceneActorReaction_ResolvePlacementDamage sub_081363F4
u8 SceneActorReaction_ApplyLinkedAreaDamage(void *);
u8 SceneActorReaction_TryBeginControl29(void *);
void SceneActorPlacement_BeginDestinationJump(void *);
u8 SceneActorReaction_ApplyEscalatingHit(void *);
void SceneActorReaction_DefeatRecordStats(void *);
void SceneActorContacts_ReleaseNode(void *);

static inline void ResetMotion(u8 *control)
{
    *(u32 *)(control + 0x18) = 0;
    *(u16 *)(control + 0x14) = 0;
    *(u16 *)(control + 8) = 0;
}

static inline u32 HitFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 HasPendingHitSave(struct SceneActorRegistryResetView *actor, u16 **out)
{
    u16 *address = &actor->field2E6;
    u32 value = *address;
    *out = address;
    if (!value)
        return 0;
    return 1;
}

#ifdef NONMATCHING
// fuzzy: 28.3%
// fix.py best (sweep after the inlined-insn fix)
// Full reconstruction; route reset, shared hit flags, HUD argument ordering and pointer lifetimes still differ.
inline static u32 Arg1(u32 k_, u16 *flags2D4)
{
    return (*flags2D4) & k_;
}

u8 SceneActorReaction_ResolvePlacementDamage(struct SceneActorRegistryResetView *actor)
{
    u8 *group = ((u8 *)actor) + 0x3D4;
    u16 *pending;
    u16 *flags2D4;
    u32 *flags2CC;
    u32 *flags2D8;
    s16 *health;
    u32 mode;
    register struct ContactCollider *collider asm("sl");
    if (HitFlag(actor, 0x4000)) {
    fail:
        return 0;
    }
    if (!HasPendingHitSave(actor, &pending)) {
        if (!SceneActorReaction_ApplyLinkedAreaDamage(actor))
            goto fail;
        {
            u8 *control = group + 0x4C;
            flags2CC = &actor->flags2CC;
            *flags2CC &= 0xFFFEBE3F;
            ResetMotion(control);
        }
        flags2D4 = &actor->flags2D4;
        {
            register u32 bit asm("sb") = 0x1000;
            if ((*flags2D4) & bit) {
                void **node = (void **)(((u8 *)actor) + 0x45C);
                SceneActorContacts_ReleaseNode(*node);
                asm("" : "+r"(bit));
                *flags2D4 &= ~bit;
                *node = (void *)0;
            }
        }
        UpdateHitFlagsShared(actor, flags2D4, flags2CC);
        SceneActor_ClearReactionFlags(actor);
        return 1;
    }
    if (SceneActorBehavior_DispatchFlaggedCategory(actor))
        return 1;
    if (SceneActorReaction_TryBeginControl29(actor))
        return 1;
    SceneActor_ClearReactionFlags(actor);
    SceneActorMovement_ProjectProbe(actor, *((u16 *)(((u8 *)actor) + 0x174)),
                                    *((u16 *)(((u8 *)actor) + 0x174)));
    SceneActorPlacement_BeginDestinationJump(actor);
    {
        u32 damage = actor->reactionSoundVariant;
        health = &actor->health;
        *health -= damage;
    }
    {
        u32 applied = SceneActorReaction_ApplyEscalatingHit(actor);
        flags2D4 = &actor->flags2D4;
        if ((!applied) && (!(actor->field2D8 & 0x20000))) {
            u32 bit = 0x20;
            *flags2D4 = bit | (*flags2D4);
        }
    }
    {
        u32 bits = (*pending) & 4;
        mode = 1;
        if (bits)
            mode = 4;
    }
    flags2D8 = &actor->field2D8;
    {
        u32 blocked = (*flags2D8) & 0x20000;
        flags2CC = &actor->flags2CC;
        collider = &actor->colliderA;
        if (!blocked) {
            if ((mode & 4) && (!Arg1(4, flags2D4))) {
                SceneActorBehavior_PlayReactionSound(actor);
                *((u16 *)(((u8 *)actor) + 0x30C)) = 12;
            }
            flags2CC = &actor->flags2CC;
            collider = &actor->colliderA;
            if (mode) {
                u32 show;
                u16 bank = *((u16 *)(((u8 *)actor) + 0x1E2));
                if (actor->drawableKind == 0)
                    AnimData_SelectPaletteBank(((u8 *)actor->drawable) + 0x2C, bank);
                else
                    *((u8 **)(((u8 *)actor->drawable) + 0x68)) = gPaletteBankData + (bank * 32);
                {
                    u32 eight = 8;
                    actor->field1E8 = eight;
                }
                flags2CC = &actor->flags2CC;
                if ((mode & 4) && (actor->category == 0x19)) {
                    u32 bit = 0x2000000;
                    *flags2CC |= bit;
                    actor->field1EA = 0x41;
                }
                {
                    u32 bit = 0x8000;
                    *flags2CC |= bit;
                }
                show = ShouldShowDamage(actor);
                collider = &actor->colliderA;
                asm("" : "+r"(collider));
                if (show) {
                    u32 zero;
                    u32 buffer[2];
                    register u32 one asm("sb");
                    s32 h = *health;
                    s32 maxh = *((u16 *)(((u8 *)actor) + 0xB0));
                    s32 kind = *((s16 *)(((u8 *)actor) + 0xB2));
                    s32 param = *((s16 *)(((u8 *)actor) + 0x9A));
                    one = 1;
                    asm("" : "+r"(one));
                    SceneHud_SetTimedState(actor->id, h, maxh, kind, mode >> 2, param);
                    Fill(&zero, buffer);
                    if (actor->reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                        DamageNumberDisplay_Show(&actor->colliderA, actor->reactionSoundVariant, 0,
                                                 &actor->position, buffer, one);
                }
            }
        }
    }
    {
        u32 flags = *flags2D4;
        if ((flags & 0x10) || ((*flags2CC) & 0x200))
            actor->fieldAA = 0;
        else if (flags & 6) {
            s32 current = (s16)actor->fieldAA;
            s32 initial = actor->fieldA8;
            if (current >= initial)
                actor->fieldAA = initial;
        }
    }
    {
        u32 keep = (*flags2CC) & 0x80;
        u8 *control = group + 0x4C;
        *flags2CC &= 0xFFFEBE3F;
        ResetMotion(control);
        *flags2CC |= keep;
    }
    SceneActorReaction_DefeatRecordStats(actor);
    UpdateHitFlagsShared(actor, flags2D4, flags2CC);
    SceneActorTerrain_ClearCollisionTimers(actor);
    if (Test(actor->flags2C0, 0x40))
        collider->flags &= ~0x100;
    else if ((actor->field2DC & 0x84000) || ((*flags2D4) & 0x2016))
        collider->flags &= ~0x100;
    else
        collider->flags |= 0x100;
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081363F4);
#endif
