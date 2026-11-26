#include "scene/collision/scene_actor_damage_visibility.h"
#include "global.h"
// confidence=low: emits a defeat burst, runs the optional script and restores idle control.
#include "scene/actor/scene_actor_reaction.h"
#include "engine/math/monster_particle_random.h"
#include "effects/particles/spark_particle.h"
#include "scene/actor/scene_actor_record.h"
#include "scene/actor/scene_actor_behavior.h"
#include "script/script_utility.h"
#include "engine/runtime.h"
#include "scene/actor/scene_actor_control_helpers.h"
void SceneActorPlacement_PlayCategoryEventSound(void *);
void ActorParticles_SpawnWideBurst(void *);
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);

static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? actor->base.motionFlags & mask : actor->base.statusFlags & mask;
}

static inline void ResetMotionControl(struct ChaseActor *actor)
{
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void InstallIdleControl(struct ChaseActor *actor, void *callback, u32 kind,
                                      u32 prepared)
{
    actor->base.field2B2 = prepared;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    ResetMotionControl(actor);
}

static inline void SelectState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}

static inline void ResetStateEntry(struct ChaseActor *actor, u32 prepared)
{
    actor->base.counter2AC = 0;
    actor->base.field2B1 = prepared;
}

static inline u8 NextSparkCount(const u16 *table)
{
    return (ParticleRandom_ReadSample(table) & 3) + 1;
}

static inline void SpawnDefeatSparks(struct ChaseActor *actor, const struct ContactPosition *range,
                                     const u16 *table, u8 firstCount, s16 height, s16 kind,
                                     s16 priority)
{
    SparkParticle_Spawn(&actor->base.body.position, firstCount, range, 32, 4, height, kind,
                        priority);
    SparkParticle_Spawn(&actor->base.body.position, NextSparkCount(table), range, 64, 4, height,
                        kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, NextSparkCount(table), range, 94, 4, height,
                        kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, NextSparkCount(table), range, 128, 4, height,
                        kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, NextSparkCount(table), range, 196, 4, height,
                        kind, priority);
    SparkParticle_Spawn(&actor->base.body.position, NextSparkCount(table), range, 240, 4, height,
                        kind, priority);
}

void SceneActorReaction_BeginDefeatBurst(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group;
    union {
        const u16 *samples;
        const u8 *category;
    } source;
    struct ContactPosition range;
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    s16 sparkHeight, sparkKind, sparkPriority;
    void *drawable;
    u32 completedState;
    group = &actor->group.common;
    completedState = 1;
    if (!SceneActorReaction_TakePrepared(actor))
        return;
    {
        SceneActorReaction_ResetRouteControl(actor, &group->control);
        {
            struct ContactCollider *collider;

            collider = &actor->base.colliderA;
            collider->flags = 4 | collider->flags;
            {
                u16 *field = &collider->responseValue;
                *field = 0;
            }
            actor->base.attachmentTimer = 0;
        }
    }
    {
        u32 showFlag = 1;
        if (actor->base.drawableKind == 0) {
            u32 *p = actor->base.drawable;
            *p |= showFlag;
            drawable = p;
        } else {
            u8 *p = actor->base.drawable;
            u8 *tile = p + 0x20;
            *(u32 *)(tile + 8) |= showFlag;
            drawable = p;
        }
    }
    sparkHeight = 20;
    sparkKind = 2;
    sparkPriority = 1;
    if (actor->base.drawableKind == 0) {
        u32 mask = ~2;
        *(u32 *)drawable &= mask;
    } else {
        u8 *tile = (u8 *)drawable + 0x20;
        u32 mask = ~2;
        *(u32 *)(tile + 8) &= mask;
    }
    {
        u32 blend = 2;
        if (actor->base.drawableKind == 0)
            *((u8 *)drawable + 7) = blend;
        else {
            u8 *field = (u8 *)drawable + 0x3A;
            *field = blend;
        }
    }
    range.x = 80;
    range.y = 60;
    range.z = 60;
    source.samples = gRandomTable;
    SpawnDefeatSparks(actor, &range, source.samples, NextSparkCount(source.samples), sparkHeight,
                      sparkKind, sparkPriority);
    if (actor->base.category == 4 || actor->base.category == 1 || actor->base.category == 5 ||
        actor->base.category == 7 || actor->base.category == 6 || actor->base.category == 24 ||
        actor->base.category == 25)
        Sound_PlaySong(229);
    SceneActorPlacement_PlayCategoryEventSound(actor);
    if (actor->base.category == 3)
        ActorParticles_SpawnWideBurst(actor);
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
    source.category = &actor->base.category;
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(*source.category - 11) > 2)
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(*source.category, actor->base.paletteOffset));
    InstallIdleControl(actor, SceneActorControl_OnIdle, 16, completedState);
    SelectState(actor, completedState, 0, 2);
    ResetStateEntry(actor, completedState);
    SceneActorRecord_RecordDefeat(actor);
    ActorShadow_Disable(actor->base.shadow);
}
