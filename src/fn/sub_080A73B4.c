#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "ram.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Select(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                          u32 flags)
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
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
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
        u32 mask = 3;
        return (mask & value) + 1;
    }
}
void TentacleBody_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    u32 variant, flags;
    struct ContactPackedVector range;
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u8 *entry = (void *)SceneActorProfiles_GetEntryAddress(
            ((struct SceneActorAnimationView *)actor)->profileId,
            ((struct SceneActorAnimationView *)actor)->profileMode, 0);
        TentacleBody_GetDirectionFrame(actor->body.direction, &variant, &flags);
        Select((void *)actor, entry[0x44], variant, flags);
        TentacleBody_ResetDirectionalAnimations(actor->context, 18);
        actor->field2B3 = 0;
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct ContactCollider *c = (void *)((u8 *)actor + 0x134);
            c->flags |= 4;
        }
    }
    if (Mod(frame, 10) == 0) {
        range.x = 60;
        range.y = 60;
        range.z = 40;
        SparkParticle_Spawn(&actor->body.position, Random4(), &range, actor->body.direction + 128, 10, 8, 2,
                            2);
    }
    TentacleBody_UpdateSegmentMotion(actor, frame);
    if (!Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30))
        return;
    actor->field2B0 = 1;
    ActorTrail_RemoveAllSegments(actor);
    TentacleActor_SpawnSparkBurst(&actor->body.position);
    {
        u32 mask = 0x400;
        if (!(actor->flags2CC & mask))
            SceneActorBehavior_CallTerrainScript((void *)actor);
    }
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = actor->field2E0;
        values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        if (!(actor->flags2D4 & mask) && (u32)(actor->category - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    }
    actor->fieldAA = 0;
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind = 16;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *p = &actor->flags114;
            u32 mask = ~1;
            *p &= mask;
        }
        actor->word104 = 0;
    }
    SetAnimation(actor, 1, 0, 2);
    ActorShadow_Disable((u8 *)actor + 0x320);
}
