#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.1%
#include "scene_actor_reaction.h"
#include "scene_actor_part_burst.h"
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
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}
static inline void ResetStateEntry(struct SceneActorRegistryResetView *actor, u32 prepared)
{
    actor->counter2AC = 0;
    actor->field2B1 = prepared;
}
// Remaining (scope 14/16): the tail matches once a `completedState = 1` set
// before TakePrepared feeds all three entry fields (as in Vorn_UpdateDefeat):
// the long-lived constant gets no register and is rebuilt at each store. Open:
// (1) the group pointer is r2 in the ROM and r4 here (the union with the table
// makes it one global pseudo); (2) in the sparks the ROM reloads the
// gRandomIndex address before each sample and keeps 20 in sl, while the
// cse-merged index pseudo (13 refs) takes sl here. A `u32 *cursor =
// &gRandomIndex;` at the top with a cursor-taking count helper fixes (2), and
// without the union (1) too (build/match/w/bdb/e7.c, scope 76/77); what is left
// there is local-alloc order in the spark block: AGBCC_PRI=table:+600 plus
// position:+10 leaves 8 insns (the phase-0 pseudo r4 vs r5 in the tail). The
// ROM's table (REG_EQUIV gRandomTable, 7 refs, priority ~680) must outrank the
// range pointer (9 refs, 1134), so it needs two more refs (9 refs -> 1184),
// which a single-set constant cannot have; position (8 refs, 1132) also needs
// one fewer insn of life or one more ref.
void SceneActorReaction_BeginDefeatBurst(struct SceneActorRegistryResetView *actor)
{
    union {
        struct SceneActorRegistryControlGroup *group;
        const u16 *table;
    } source;
    struct ContactPosition range;
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    void *drawable;
    u32 completedState;
    source.group = &actor->group.common;
    completedState = 1;
    if (!SceneActorReaction_TakePrepared(actor))
        return;
    {
        struct SceneActorRegistryControl *control = &source.group->control;

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
    source.table = (const u16 *)RANDOM_TABLE_ADDR;
    SceneActorEffects_SpawnDefeatSparks(
        actor, &range, source.table,
        SceneActorEffects_NextSparkCount((const u16 *)RANDOM_TABLE_ADDR));
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
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    InstallIdleControl(actor, SceneActorControl_OnIdle, 16, completedState);
    SelectState(actor, completedState, 0, 2);
    ResetStateEntry(actor, completedState);
    if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->body.id);
    ActorShadow_Disable(actor->shadow);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_BeginDefeatBurst);
#endif
