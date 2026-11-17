#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "spark_particle.h"
#include "monster_particle_random.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActor_CallContextScriptB(void *);
void Vorn_SendMessage(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);

static inline u16 Vorn_GetDefeatScript(struct ChaseActor *actor)
{
    return actor->base.script1F8;
}
static inline s32 Vorn_GetScriptParameter(struct ChaseActor *actor, u32 index)
{
    return actor->base.parameters1FC[index];
}
static inline u32 Vorn_ActorFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}
static inline u8 Vorn_RandomSparkCount(const u16 *table, u32 *cursor)
{
    u32 index = (*cursor + 1) & 1023;
    u32 value;
    *cursor = index;
    index *= 2;
    PARTICLE_RANDOM_READ_SAMPLE(value, (const u16 *)(index + (u32)table), 0);
    value &= 3;
    return value + 1;
}
static inline void Vorn_ClearRoute(struct ChaseActor *actor,
                                   struct SceneActorRegistryControl *control, u32 cleared)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = cleared;
    control->field14 = cleared;
    control->timer = cleared;
}

static inline void Vorn_SetTileFlags(struct SceneInputWidget *tile, u32 bits)
{
    tile->flags |= bits;
}

static inline void Vorn_InstallIdleControl(struct ChaseActor *actor, void *callback, u32 kind,
                                           u32 prepared)
{
    actor->base.field2B2 = prepared;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    SceneActorChase_ResetMotionControl(actor);
}
static inline void Vorn_ResetStateEntry(struct ChaseActor *actor, u32 prepared)
{
    actor->base.counter2AC = 0;
    actor->base.field2B1 = prepared;
}
union VornSpreadVector {
    struct ContactPackedVector bits;
    u32 words[2];
};
/* Completes the defeat sequence, dispatches its script, and enters idle control. */
void Vorn_UpdateDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    union VornSpreadVector range;
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 workValue;
    u32 completedState;
    u32 *cursor = &gRandomIndex;
    completedState = 1;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 10);
        {
            u8 *entry = &actor->base.restart;
            workValue = 0;
            *entry = workValue;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8))
            Sound_PlaySong(0x12A);
        Vorn_ClearRoute(actor, &actor->group.common.control, workValue);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            collider->flags |= 4;
        }
        Vorn_SendMessage(actor);
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    if (!Vorn_ActorFlags(actor, 0x400)) {
        u8 category;
        SceneActor_CallContextScriptB(actor);
        {
            union VornSpreadVector *spread;
            const struct ContactPosition *origin;
            u32 sparkKind;
            range.bits.x = 80;
            range.bits.y = 60;
            spread = &range;
            /* Preserve the unused upper half of the packed depth word. */
            spread->words[1] = (spread->words[1] & 0xFFFF0000) | 60;
            workValue = (u32)gRandomTable;
            SparkParticle_Spawn(
                (origin = (const struct ContactPosition *)&actor->base.body.position),
                Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0x20, 4, 20, (sparkKind = 2), 1);
            /* Keep the three shared spark values distinct through CSE. */
            sparkKind |= ((u32)actor & 255) >> 8;
            spread = (union VornSpreadVector *)((u32)spread + (((u32)actor & 31) >> 5));
            origin = (const struct ContactPosition *)((u32)origin + (((u32)actor & 127) >> 7));
            SparkParticle_Spawn(origin, Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                                (const struct ContactPosition *)spread, 0x40, 4, 20, sparkKind, 1);
            SparkParticle_Spawn(origin, Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                                (const struct ContactPosition *)spread, 0x5E, 4, 20, sparkKind, 1);
            SparkParticle_Spawn(origin, Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                                (const struct ContactPosition *)spread, 0x80, 4, 20, sparkKind, 1);
            SparkParticle_Spawn(origin, Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                                (const struct ContactPosition *)spread, 0xC4, 4, 20, sparkKind, 1);
            SparkParticle_Spawn(origin, Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                                (const struct ContactPosition *)spread, 0xF0, 4, 20, sparkKind, 1);
        }
        category = actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    script = Vorn_GetDefeatScript(actor);
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = Vorn_GetScriptParameter(actor, 0);
        values[4] = Vorn_GetScriptParameter(actor, 1);
        values[5] = Vorn_GetScriptParameter(actor, 2);
        values[6] = actor->base.hitOwners;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x4000) &&
        (u32)(actor->base.category - 11) > 2)
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    actor->base.attachmentTimer = 0;
    {
        u32 mask = 1;
        if (!actor->base.drawableKind)
            *((u32 *)actor->base.drawable) |= mask;
        else
            Vorn_SetTileFlags(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, mask);
    }
    Vorn_InstallIdleControl(actor, SceneActorControl_OnIdle, 16, completedState);
    SceneActorChase_SelectState(actor, completedState, 0, 2);
    Vorn_ResetStateEntry(actor, completedState);
    ActorShadow_Disable(actor->base.shadow);
}
