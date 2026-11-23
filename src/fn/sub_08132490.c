#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_damage_visibility.h"
#include "scene_actor_damage_flash.h"
#include "scene_actor_drawable_flags.h"
#include "scene_actor_reaction.h"
#include "scene_actor_palette.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void Sound_PlaySong(u32);

void SceneActorReaction_ApplyCategoryDamage(struct ChaseActor *actor)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    {
        u32 damage = actor->base.reactionSoundVariant;
        actor->base.health -= damage;
    }
    if (!(actor->base.hitAttributes & 0x20000)) {
        SceneActorDamage_Flash(actor);
        {
            u32 bit = 0x8000;
            actor->base.behaviorFlags |= bit;
        }
        if (SceneActorDamage_ShouldShow(actor)) {
            struct ContactPosition offset;
            s32 health = actor->base.health;
            u32 max = (u16)actor->base.defaultHealth;
            s32 kind = (s16)actor->base.hudKind;
            s32 param = (s16)actor->base.hudLabel;
            SceneHud_SetTimedState(actor->base.body.id, health, max, kind, 0, param);
            ZeroFill8(&offset);
            if (actor->base.reactionSoundVariant || !(actor->base.hitAttributes & 0x20000))
                DamageNumberDisplay_Show(&actor->base.colliderA, actor->base.reactionSoundVariant,
                                         0, &actor->base.body.position, &offset, 1);
        }
    }
    SceneActorMovement_ProjectProbe(actor, 0, 0);
    {
        u16 *flags = &actor->base.motionFlags;
        u32 mask = ~0x2000;
        u16 value = mask & *flags;
        u32 zero = 0;
        *flags = value;
        actor->base.hitLockTimer = zero;
    }
    SceneActorTerrain_ClearCollisionTimers(actor);
    SceneActorDamage_ClearHitReaction(actor);
    if (actor->base.health <= 0) {
        collider->responseValue = 0;
        ActorStatus_ClearDrawableFlags(actor, 1);
        {
            void *control = actor->base.onDefeat;
            SceneActorTerrain_InstallControl(actor, control, 17);
        }
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        actor->base.hitOwners = 32;
        if (!SceneActorDamage_TestFlags(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    } else {
        Sound_PlaySong(0x13E);
    }
}
