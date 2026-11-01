#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.1%
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
static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = (void *)SceneActorControl_OnIdle;
    u8 kind = 16;
    u32 zero;
    {
        u32 preparedOffset = 0x2B2;
        u8 *field;
        u8 one;
        field = (u8 *)actor + preparedOffset;
        zero = 0;
        one = 1;
        *field = one;
    }
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    {
        u32 off = 0x2C8;
        {
            u16 *field = (void *)((u8 *)actor + off);
            *field = zero;
        }
    }
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = zero;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// Remaining (scope 38/37): in the sparks the ROM spills the gRandomIndex
// address (reloaded before each sample) and keeps 20/2/1 in sl/sb/r8; the draft's
// cse-merged index pseudo (13 refs, priority 1741) outranks the REG_EQUIV
// constants (~670) and takes sl. Splitting the first sample's address did not
// lower it. Also open: the group pointer's register at the top and the
// SetControl/SetAnimation constants the ROM rebuilds instead of sharing.
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
    source.group = &actor->group.common;
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
    SetControl(actor);
    SetAnimation(actor, 1, 0, 2);
    if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->body.id);
    ActorShadow_Disable(actor->shadow);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_BeginDefeatBurst);
#endif
