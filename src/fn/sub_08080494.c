#include "global.h"
#ifdef NONMATCHING
// fuzzy: 99.0%
#include "scene_actor_reaction.h"
#include "scene_actor_part_burst.h"
#include "monster_particle_random.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "runtime.h"
void SceneActorPlacement_PlayCategoryEventSound(void *);
void ActorParticles_SpawnWideBurst(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
#include "scene_actor_record.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}
#define STORE_STATE_BYTE(member, value)                                                            \
    do {                                                                                           \
        (member) = (value);                                                                        \
    } while (0)
static inline void ResetMotionControl(struct SceneActorRegistryResetView *actor)
{
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void InstallIdleControl(struct SceneActorRegistryResetView *actor, void *callback,
                                      u32 kind, u32 prepared)
{
    actor->field2B2 = prepared;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    ResetMotionControl(actor);
}
static inline void SelectState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                               u8 behavior)
{
    STORE_STATE_BYTE(actor->state2A8, state);
    STORE_STATE_BYTE(actor->phase2A9, phase);
    STORE_STATE_BYTE(actor->behavior2AA, behavior);
}
static inline void ResetStateEntry(struct SceneActorRegistryResetView *actor, u32 prepared)
{
    actor->counter2AC = 0;
    actor->field2B1 = prepared;
}
static inline u8 NextSparkCount(const u16 *table, u32 *cursor)
{
    u32 index = (*cursor + 1) & 1023;
    u32 value;
    *cursor = index;
    PARTICLE_RANDOM_READ_SAMPLE(value, table, index);
    value &= 3;
    return value + 1;
}
#define BURST_ORIGIN(actor)                                                                        \
    ({                                                                                             \
        const struct ContactPosition *position;                                                    \
        PARTICLE_RANDOM_SAVE_ORIGIN(position, &(actor)->body.position);                            \
        position;                                                                                  \
    })
void SceneActorReaction_BeginDefeatBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group;
    const u16 *table;
    struct ContactPosition range;
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    void *drawable;
    u32 *cursor = &gRandomIndex;
    u32 completedState;
    u8 phase;
    group = &actor->group.common;
    completedState = 1;
    if (!SceneActorReaction_TakePrepared(actor))
        return;
    {
        struct SceneActorRegistryControl *control = &group->control;

        u32 *field;
        u32 mask;
        u32 zero;
        field = &actor->flags2CC;
        mask = ~0x141C0;
        *field &= mask;
        zero = 0;
        control->counter = zero;
        control->field14 = zero;
        control->timer = zero;
        {
            struct ContactCollider *collider;

            u32 bit;
            collider = &actor->colliderA;
            bit = 4;
            collider->flags = bit | collider->flags;
            {
                u16 *field = &collider->responseValue;
                *field = zero;
            }
            actor->fieldAA = zero;
        }
    }
    {
        u32 showFlag = 1;
        if (actor->drawableKind == 0) {
            u32 *p = actor->drawable;
            *p |= showFlag;
            drawable = p;
        } else {
            u8 *p = actor->drawable;
            u8 *tile = p + 0x20;
            *(u32 *)(tile + 8) |= showFlag;
            drawable = p;
        }
    }
    if (actor->drawableKind == 0) {
        u32 mask = ~2;
        *(u32 *)drawable &= mask;
    } else {
        u8 *tile = (u8 *)drawable + 0x20;
        u32 mask = ~2;
        *(u32 *)(tile + 8) &= mask;
    }
    {
        u32 blend = 2;
        if (actor->drawableKind == 0)
            *((u8 *)drawable + 7) = blend;
        else {
            u8 *field = (u8 *)drawable + 0x3A;
            *field = blend;
        }
    }
    range.x = 80;
    range.y = 60;
    range.z = 60;
    table = gRandomTable;
    {
        s32 verticalRange, kind;
        u32 priority;
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 32, 4,
                            verticalRange = 20, kind = 2, priority = 1);
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 64, 4,
                            verticalRange, kind, priority);
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 94, 4,
                            verticalRange, kind, priority);
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 128, 4,
                            verticalRange, kind, priority);
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 196, 4,
                            verticalRange, kind, priority);
        SparkParticle_Spawn(BURST_ORIGIN(actor), NextSparkCount(table, cursor), &range, 240, 4,
                            verticalRange, kind, priority);
    }
    if (actor->category == 4 || actor->category == 1 || actor->category == 5 ||
        actor->category == 7 || actor->category == 6 || actor->category == 24 ||
        actor->category == 25)
        Sound_PlaySong(229);
    SceneActorPlacement_PlayCategoryEventSound(actor);
    if (actor->category == 3)
        ActorParticles_SpawnWideBurst(actor);
    script = (u16)((struct SceneActorBehaviorView *)actor)->script1F8;
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[0];
        values[4] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[1];
        values[5] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[2];
        values[6] = actor->field2E0;
        values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    phase = 0;
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    InstallIdleControl(actor, SceneActorControl_OnIdle, 16, completedState);
    SelectState(actor, completedState, phase, 2);
    ResetStateEntry(actor, completedState);
    if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->body.id);
    ActorShadow_Disable(actor->shadow);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_BeginDefeatBurst);
#endif
