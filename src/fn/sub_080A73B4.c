#include "scene_actor_availability.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_terrain_reaction.h"
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(struct RoomActor *);
void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void Select(struct RoomActor *actor, u32 selection, u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->animationIndex;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mode;
            if (flags & 4)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

s32 Mod(s32, s32);
void TentacleBody_GetDirectionFrame(u8, u32 *, u32 *);
void TentacleBody_ResetDirectionalAnimations(void *, s32);
void TentacleBody_UpdateSegmentMotion(void *, s32);
void ActorTrail_RemoveAllSegments(void *);
void TentacleActor_SpawnSparkBurst(void *);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
static inline u32 Random4(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 value = *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
        return (3 & value) + 1;
    }
}
void TentacleBody_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    u32 variant, flags;
    struct ContactPackedVector range;
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u8 *entry = (void *)SceneActorProfiles_GetEntryAddress(actor->base.profileId,
                                                               actor->base.profileMode, 0);
        TentacleBody_GetDirectionFrame(actor->base.body.direction, &variant, &flags);
        Select((void *)actor, entry[0x44], variant, flags);
        TentacleBody_ResetDirectionalAnimations(actor->base.context, 18);
        actor->base.restart = 0;
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct ContactCollider *c = &actor->base.colliderA;
            c->flags |= 4;
        }
    }
    if (Mod(frame, 10) == 0) {
        range.x = 60;
        range.y = 60;
        range.z = 40;
        SparkParticle_Spawn(&actor->base.body.position, Random4(), &range,
                            actor->base.body.direction + 128, 10, 8, 2, 2);
    }
    TentacleBody_UpdateSegmentMotion(actor, frame);
    if (!Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30))
        return;
    actor->base.stepDone = 1;
    ActorTrail_RemoveAllSegments(actor);
    TentacleActor_SpawnSparkBurst(&actor->base.body.position);
    if (!(Flags_MaskedWord(0x400, &actor->base.behaviorFlags)))
        SceneActorBehavior_CallTerrainScript(&actor->base);
    script = (u16)actor->base.script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.parameters1FC[0];
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        values[6] = actor->base.hitOwners;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x4000) &&
            (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    }
    actor->base.attachmentTimer = 0;
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorTerrain_EnterState(actor, 1, 0, 2);
    ActorShadow_Disable(actor->base.shadow);
}
