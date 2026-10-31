#define SCENE_ACTOR_CHASE_WIDE_STATE 1
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
#define VORN_READ_RANDOM_SAMPLE(out_, samples_, offset_)                                           \
    do {                                                                                           \
        (out_) = *(const u16 *)((offset_) + (u32)(samples_));                                      \
    } while (0)
static inline u8 Vorn_RandomSparkCount(const u16 *table, u32 *cursor)
{
    u32 index = (*cursor + 1) & 1023;
    u32 value;
    *cursor = index;
    index *= 2;
    VORN_READ_RANDOM_SAMPLE(value, table, index);
    value &= 3;
    return value + 1;
}
#define VORN_STORE_ENTRY_BYTE(field_, value_)                                                      \
    do {                                                                                           \
        *(field_) = (value_);                                                                      \
    } while (0)
#define VORN_CLEAR_ROUTE_FIELDS(control_, zero_)                                                   \
    do {                                                                                           \
        (control_)->counter = (zero_);                                                             \
        (control_)->field14 = (zero_);                                                             \
        (control_)->timer = (zero_);                                                               \
    } while (0)
#define VORN_CAPTURE_SPARK_ORIGIN(out_, actor_)                                                    \
    do {                                                                                           \
        (out_) = (const struct ContactPosition *)&(actor_)->body.position;                         \
    } while (0)
#define VORN_SPARK_ORIGIN(actor_)                                                                  \
    ({                                                                                             \
        const struct ContactPosition *position;                                                    \
        VORN_CAPTURE_SPARK_ORIGIN(position, actor_);                                               \
        position;                                                                                  \
    })
#define VORN_RESET_ENTRY_FIELDS(actor_, prepared_)                                                 \
    do {                                                                                           \
        (actor_)->counter2AC = 0;                                                                  \
        (actor_)->field2B1 = (prepared_);                                                          \
    } while (0)
#define VORN_COMPLETE_DEFEAT(actor_, state_)                                                       \
    do {                                                                                           \
        Vorn_InstallIdleControl((actor_), SceneActorControl_OnIdle, 16, 1);                        \
        SceneActorChase_SelectState((actor_), (state_), 0, 2);                                     \
        Vorn_ResetStateEntry((actor_), 1);                                                         \
    } while (0)
static inline void Vorn_ClearRoute(struct SceneActorRegistryResetView *actor,
                                   struct SceneActorRegistryControl *control, u32 cleared)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    VORN_CLEAR_ROUTE_FIELDS(control, cleared);
}

static inline void Vorn_SetTileFlags(struct SceneActorAnimationTile *tile, u32 bits)
{
    tile->flags |= bits;
}

#define VORN_PREPARE_ENTRY_BYTE(actor_, prepared_)                                                 \
    do {                                                                                           \
        (actor_)->field2B2 = (prepared_);                                                          \
        (actor_)->field2B0 = 0;                                                                    \
    } while (0)
static inline void Vorn_InstallIdleControl(struct SceneActorRegistryResetView *actor,
                                           void *callback, u32 kind, u32 prepared)
{
    VORN_PREPARE_ENTRY_BYTE(actor, prepared);
    actor->field2B5 = kind;
    actor->target = callback;
    SceneActorChase_ResetMotionControl(actor);
}
static inline void Vorn_ResetStateEntry(struct SceneActorRegistryResetView *actor, u32 prepared)
{
    VORN_RESET_ENTRY_FIELDS(actor, prepared);
}
#define VORN_PREPARE_COMPLETED_STATE(state_)                                                       \
    do {                                                                                           \
        (state_) = 1;                                                                              \
    } while (0)
#define VORN_SET_SPARK_KIND(out_, kind_)                                                           \
    do {                                                                                           \
        (out_) = (kind_);                                                                          \
    } while (0)
#define VORN_DEFEAT_SPARK_KIND()                                                                   \
    ({                                                                                             \
        s32 kind;                                                                                  \
        VORN_SET_SPARK_KIND(kind, 2);                                                              \
        kind;                                                                                      \
    })
#define VORN_CAPTURE_SPARK_SPREAD(out_, range_)                                                    \
    do {                                                                                           \
        (out_) = &(range_);                                                                        \
    } while (0)
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
    VORN_PREPARE_COMPLETED_STATE(completedState);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 10);
        {
            u8 *entry = &actor->field2B3;
            workValue = 0;
            VORN_STORE_ENTRY_BYTE(entry, workValue);
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
            VORN_CAPTURE_SPARK_SPREAD(spread, range);
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
    VORN_COMPLETE_DEFEAT(actor, completedState);
    ActorShadow_Disable(actor->shadow);
}
