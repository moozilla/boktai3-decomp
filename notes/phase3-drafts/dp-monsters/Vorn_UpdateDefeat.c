#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "spark_particle.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActor_CallContextScriptB(void *);
void Vorn_SendMessage(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);

struct VornDefeatScript {
    u16 id, unused;
    s32 parameters[3];
};
static inline u16 Vorn_GetDefeatScript(struct SceneActorRegistryResetView *actor)
{
    struct VornDefeatScript *script = (void *)&actor->unknown1F4[4];
    return script->id;
}
static inline s32 Vorn_GetScriptParameter(struct SceneActorRegistryResetView *actor, u32 index)
{
    return ((struct SceneActorBehaviorView *)actor)->parameters1FC[index];
}
static inline u32 Vorn_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline u8 Vorn_RandomSparkCount(const u16 *table, u32 *cursor)
{
    u32 index = (*cursor + 1) & 1023;
    u32 value;
    *cursor = index;
    index *= 2;
    value = *(const u16 *)(index + (u32)table);
    value &= 3;
    return value + 1;
}
#define VORN_SPARK_ORIGIN(actor_)                                                                  \
    ({                                                                                             \
        const struct ContactPosition *position;                                                    \
        position = (const struct ContactPosition *)&(actor_)->body.position;                                               \
        position;                                                                                  \
    })
static inline void Vorn_ClearRoute(struct SceneActorRegistryResetView *actor,
                                   struct SceneActorRegistryControl *control, u32 cleared)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = cleared;
    control->field14 = cleared;
    control->timer = cleared;
}

static inline void Vorn_SetTileFlags(struct SceneActorAnimationTile *tile, u32 bits)
{
    tile->flags |= bits;
}

static inline void Vorn_InstallIdleControl(struct SceneActorRegistryResetView *actor,
                                           void *callback, u32 kind, u32 prepared)
{
    actor->field2B2 = prepared;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    SceneActorChase_ResetMotionControl(actor);
}
static inline void Vorn_ResetStateEntry(struct SceneActorRegistryResetView *actor, u32 prepared)
{
    actor->counter2AC = 0;
    actor->field2B1 = prepared;
}
#define VORN_DEFEAT_SPARK_KIND()                                                                   \
    ({                                                                                             \
        s32 kind;                                                                                  \
        kind = 2;                                                              \
        kind;                                                                                      \
    })
union VornSpreadVector {
    struct ContactPackedVector bits;
    u32 words[2];
};
/* Completes the defeat sequence, dispatches its script, and enters idle control. */
void Vorn_UpdateDefeat(struct SceneActorRegistryResetView *actor)
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
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 10);
        {
            u8 *entry = &actor->field2B3;
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
            struct ContactCollider *collider = &actor->colliderA;
            collider->flags |= 4;
        }
        Vorn_SendMessage(actor);
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    if (!Vorn_ActorFlags(actor, 0x400)) {
        u8 category;
        SceneActor_CallContextScriptB(actor);
        {
            union VornSpreadVector *spread;
            range.bits.x = 80;
            range.bits.y = 60;
            spread = &range;
            /* Preserve the unused upper half of the packed depth word. */
            spread->words[1] = (spread->words[1] & 0xFFFF0000) | 60;
            workValue = (u32)gRandomTable;
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0x20, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0x40, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0x5E, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0x80, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0xC4, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
            SparkParticle_Spawn(
                VORN_SPARK_ORIGIN(actor), Vorn_RandomSparkCount((const u16 *)workValue, cursor),
                (const struct ContactPosition *)spread, 0xF0, 4, 20, VORN_DEFEAT_SPARK_KIND(), 1);
        }
        category = actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    script = Vorn_GetDefeatScript(actor);
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = Vorn_GetScriptParameter(actor, 0);
        values[4] = Vorn_GetScriptParameter(actor, 1);
        values[5] = Vorn_GetScriptParameter(actor, 2);
        values[6] = actor->field2E0;
        values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x4000) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    actor->fieldAA = 0;
    {
        u32 mask = 1;
        if (!actor->drawableKind)
            *((u32 *)actor->drawable) |= mask;
        else
            Vorn_SetTileFlags(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              mask);
    }
    Vorn_InstallIdleControl(actor, SceneActorControl_OnIdle, 16, 1);
    SceneActorChase_SelectState(actor, completedState, 0, 2);
    Vorn_ResetStateEntry(actor, 1);
    ActorShadow_Disable(actor->shadow);
}
