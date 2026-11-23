#ifndef GUARD_SCENE_ACTOR_DAMAGE_FLASH_H
#define GUARD_SCENE_ACTOR_DAMAGE_FLASH_H

#include "scene_actor_palette.h"
#include "scene_actor_state.h"

void AnimData_SelectPaletteBank(void *, s32);

/* A damaged actor switches to its alternate palette bank and flashes for eight frames. */
static inline void SceneActorDamage_Flash(struct ChaseActor *actor)
{
    u16 bank = actor->base.paletteAlternateBank;
    if (actor->base.drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, bank);
    else
        ((struct SceneActorPaletteTileView *)actor->base.drawable)->palette =
            (const u16 *)(gPaletteBankData + bank * 32);
    {
        u32 frames = 8;
        actor->base.flashTimer = frames;
    }
}

/* Behavior flag 0x8000 marks the actor as flashing from a hit. */
static inline void SceneActorDamage_MarkFlashing(u32 *behaviorFlags)
{
    u32 bit = 0x8000;
    *behaviorFlags |= bit;
}

/* Ending a hit clears the reaction bits 0x980. */
static inline void SceneActorDamage_ClearHitReaction(struct ChaseActor *actor)
{
    u16 *flags = &actor->base.reactionFlags;
    u32 mask = ~0x980;
    *flags = mask & *flags;
}

/* A fatal hit leaves only the down bit (0x40) and no pending hit. */
static inline void SceneActorDamage_MarkDown(struct ChaseActor *actor, u16 *flags2D4)
{
    u32 zero = 0;
    *flags2D4 = 0x40;
    actor->base.field2E6 = zero;
    SceneActorDamage_ClearHitReaction(actor);
}

/* Clears 0x201 from the hit flags just read; the mask lives in the low half of a word whose upper
   half still holds the flags. */
static inline void SceneActorDamage_ClearHitStart(u16 *flags2D4, u32 flags)
{
    union {
        u32 raw;
        struct {
            s32 bits : 16;
            u32 upper : 16;
        } field;
    } mask;
    u32 value;
    mask.raw = flags;
    mask.field.bits = ~0x201;
    value = mask.field.bits & flags;
    *flags2D4 = value;
}

/* A hit that is not fatal: drop the guard and tracking bits, keep 0x80 only for hits that carry
   0x20, clear the stagger status, mark the reaction and record the hit as pending. */
static inline void SceneActorDamage_AcceptHit(struct ChaseActor *actor, u16 *flags2D4,
                                              u32 *flags2CC, u32 down)
{
    {
        u32 mask = ~0x4000;
        *flags2CC &= mask;
    }
    {
        struct SceneActorChaseTracking *tracking = &actor->base.tracking;
        s32 mask = ~0x10;
        tracking->flags &= mask;
    }
    {
        u32 test = 0x20;
        if (!(*flags2D4 & test)) {
            u32 mask = ~0x80;
            *flags2CC &= mask;
        }
    }
    {
        u32 *flags2D0 = &actor->base.statusFlags;
        u32 mask = 0xFFEFFFFB;
        *flags2D0 &= mask;
    }
    {
        u32 bit = 0x400;
        actor->base.reactionFlags |= bit;
    }
    actor->base.field2E6 = down;
    {
        u32 bit = 0x2000;
        *flags2CC |= bit;
    }
}

#endif
