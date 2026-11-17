#include "global.h"
#include "scene_actor_damage_visibility.h"
#include "scene_actor_reaction.h"
#include "scene_actor_palette.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}
static inline void Reaction_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void Hide(struct ChaseActor *a)
{
    if (!a->base.drawableKind) {
        u32 *flags = a->base.drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneInputWidget *tile = (void *)((u8 *)a->base.drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorReaction_PlayStaggerBurst(void);

struct FlagHalfword {
    s16 bits : 16;
};
static inline void ClearHit(struct FlagHalfword *flags, u32 mask)
{
    flags->bits = mask & flags->bits;
}
static inline void SetHit(s16 bit, struct FlagHalfword *p)
{
    p->bits = bit | p->bits;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
void SceneActorReaction_ApplyStaggerDamage(struct ChaseActor *actor)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    {
        u32 damage = actor->base.reactionSoundVariant;
        actor->base.health -= damage;
    }
    if (!(actor->base.hitAttributes & 0x20000)) {
        u16 bank = actor->base.paletteAlternateBank;
        if (actor->base.drawableKind == 0)
            AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, bank);
        else
            ((struct SceneActorPaletteTileView *)actor->base.drawable)->palette =
                (const u16 *)(gPaletteBankData + bank * 32);
        {
            u32 eight = 8;
            actor->base.flashTimer = eight;
        }
        {
            u32 bit = 0x8000;
            actor->base.behaviorFlags |= bit;
        }
        if (SceneActorDamage_ShouldShow(actor)) {
            u32 zero;
            u32 buffer[2];
            s32 health = actor->base.health;
            u32 max = (u16)actor->base.defaultHealth;
            s32 kind = (s16)actor->base.hudKind;
            s32 param = (s16)actor->base.hudLabel;
            SceneHud_SetTimedState(actor->base.body.id, health, max, kind, 0, param);
            Fill(&zero, buffer);
            if (actor->base.reactionSoundVariant || !(actor->base.hitAttributes & 0x20000))
                DamageNumberDisplay_Show(&actor->base.colliderA, actor->base.reactionSoundVariant,
                                         0, &actor->base.body.position, buffer, 1);
        }
    }
    SceneActorMovement_ProjectProbe(actor, 0, 0);
    {
        struct FlagHalfword *flags = (void *)&actor->base.motionFlags;
        ClearHit(flags, ~0x2000);
        actor->base.hitLockTimer = 0;
    }
    SceneActorTerrain_ClearCollisionTimers(actor);
    {
        u16 *flags = &actor->base.reactionFlags;
        u32 mask = ~0x980;
        *flags = mask & *flags;
    }
    if (actor->base.health <= 0) {
        collider->responseValue = 0;
        Hide(actor);
        {
            void *control = actor->base.onDefeat;
            Reaction_SetControl(actor, 17, control);
        }
        {
            u32 state = 3;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = 0;
            actor->base.behavior2AA = 0;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
        }
        actor->base.hitOwners = 32;
        if (!Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    } else {
        SetHit(0x20, (void *)&actor->base.motionFlags);
        {
            void *control = SceneActorReaction_PlayStaggerBurst;
            Reaction_SetControl(actor, 0x21, control);
        }
        {
            u32 state = 3;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = 0;
            actor->base.behavior2AA = 0;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
        }
    }
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
}
