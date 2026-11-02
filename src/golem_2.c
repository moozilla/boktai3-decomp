// MATCHING_CONCESSION: register-pinning
#include "golem_2.h"
#include "script_utility.h"
#include "engine.h"
#include "player.h"
#include "ram.h"

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void Golem_ResetIdleControl(u8 *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u32 Flags16(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}

void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorTerrain_ClearCollisionTimers(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
void ActorShadow_Disable(void *);
void ActorParticles_SpawnWideBurst(void *);
void SceneActorControl_OnIdle(void);
void Golem_OnUnusedState(void)
{
}

static inline u32 Flags32At(struct SceneActorRegistryResetView *a, u32 mask)
{
    return *(u32 *)((u8 *)a + 0x2CC) & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)table + index);
    v &= 3;
    return v + 1;
}

/* Releases the defeated golem, runs its script, and emits the defeat burst. */
void Golem_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    u8 *group = (u8 *)&actor->group.common;
    u16 script;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *(u16 *)((u8 *)control + 0x14) = 0;
            control->timer = 0;
        }
        Sound_PlaySong(0x13E);
        {
            u16 *flags = (u16 *)((u8 *)actor + 0x13A);
            u32 value = *flags;
            u32 m = 4;
            m |= value;
            *flags = m;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = *(s32 *)((u8 *)actor + 0x2E0);
        values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!Flags16(actor, 0x4000) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    if (!Flags32At(actor, 0x400)) {
        u8 category;
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            register const u16 *table asm("sl") = gRandomTable;
            s32 duration;
            s32 scale;
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x20, 4,
                                duration = 20, scale = 2, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x40, 4,
                                duration, scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x5E, 4,
                                duration, scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x80, 4,
                                duration, scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0xC4, 4,
                                duration, scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0xF0, 4,
                                duration, scale, 1);
        }
        category = actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind = 16;
        u32 zero;
        u32 one;
        u8 *ready = &actor->field2B2;
        zero = 0;
        one = 1;
        *ready = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *field = &actor->flags114;
            s32 clear = ~1;
            *field &= clear;
        }
        actor->word104 = zero;
        {
            u32 behavior = 2;
            actor->state2A8 = one;
            actor->phase2A9 = zero;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = one;
        }
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
    if (!*((u8 *)actor + 8))
        **(u32 **)((u8 *)actor + 4) |= 1;
    else {
        struct DefeatOwnerGroup {
            u32 unknown[2];
            u32 flags;
        };
        struct DefeatOwner {
            u8 unknown[0x20];
            struct DefeatOwnerGroup group;
        };
        struct DefeatOwner *owner = *(void **)((u8 *)actor + 4);
        struct DefeatOwnerGroup *ownerGroup = &owner->group;
        ownerGroup->flags |= 1;
    }
    ActorParticles_SpawnWideBurst(actor);
    SceneActorPlacement_PlayCategoryEventSound(actor);
}

void SceneActorRoute_CommitReachableDestination(void *);

#define SCENE_ACTOR_CHASE_WIDE_STATE          1
#define SCENE_ACTOR_CHASE_FRACTION_ACCESSOR   1
#define SCENE_ACTOR_CHASE_SCALAR_CELLS        1
#define SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES 1
#include "scene_actor_chase_reaction.h"

/* Fields are established by 080C9C04 and the ordinary ReturnHome/ChargeOrReturnHome
 * relatives. Names for the response count and flags are tentative. */

struct GolemReactionBody {
    u8 unknown0[0x624];
    u8 responseCount;
    u8 unknown625[3];
    u32 flags628;
    u8 unknown62C[0x54];
    s16 routeDelay;
    u8 unknown682[0x2A];
    u16 shakeStrength, shakeTimer;
};

static inline void GolemReaction_SetBodyFlags(struct GolemReactionBody *body, u32 flags)
{
    body->flags628 |= flags;
}

static inline void GolemReaction_LatchHitReact(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    if (!actor->group.hitReactionLatch && (actor->field2E6 & 8)) {
        actor->field2B8 = 0x2D;
        actor->group.hitReactionLatch = 0x41;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
}
static inline s16 GolemReaction_CurrentHeight(struct SceneActorRegistryResetView *actor)
{
    return actor->fieldA8;
}

static inline void GolemReaction_SetChargeHeight(struct SceneActorRegistryResetView *actor,
                                                 s32 height)
{
    actor->fieldAA = height;
}

static inline void GolemReaction_EnterCharge(struct SceneActorRegistryResetView *actor, s16 height)
{
    GolemReaction_SetChargeHeight(actor, height);
    SceneActorChase_SetTypedControl(actor, actor->reactionTarget230, 0x12);
    SceneActorChase_SetTypedState(actor, 3, 0, 0);
}

static inline void GolemReaction_Settle(struct SceneActorRegistryResetView *actor)
{
    SceneActorChase_SetTypedControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetTypedState(actor, 0, 0, 7);
    actor->field2C2 = 0x20;
}

/* Dispatch the hit-reaction state; return one when entering a state. */
s32 Golem_UpdateHitReaction(struct SceneActorRegistryResetView *actor)
{
    struct GolemReactionBody *body = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    void *object = actor;

    GolemReaction_SetBodyFlags(body, 0x800);
    actor->flags2D0 &= ~0x1000000;
    SceneActorPlacement_PlayVariantEventSound(actor, 1);

    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->body.direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if (!SceneActorChase_Arrive(object)) {
            if (!SceneActorChase_TryRoute(object, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        GolemReaction_Settle(actor);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8000)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 2)) {
        actor->reactionStart = actor->reactionEnd = actor->body.position;
        SceneActorChase_SetTypedControl(actor, group->contactControl, 7);
        SceneActorChase_SetTypedState(actor, 3, 0, 0);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x100)) {
        if (actor->field2D8 & 0x400) {
            body->shakeStrength = 30;
            body->shakeTimer = 20;
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->field2DC |= 0x80000;
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
            SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000003);
            actor->field2C2 = 0x40;
            return 1;
        }
        if (actor->field2DC & 0xC0000) {
            GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
            actor->flags2D0 &= ~0x1000000;
            return 1;
        }
        if (actor->tracking.target) {
            SceneActorRoute_CommitReachableDestination(actor);
            body->routeDelay = 120;
        }
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        GolemReaction_EnterCharge(actor, GolemReaction_CurrentHeight(actor));
        actor->flags2D0 &= ~0x1000000;
        return 1;
    }

    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_TrackingFlags(&actor->tracking, 0x10))
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        GolemReaction_LatchHitReact(object);
        if (!actor->tracking.target)
            return 0;
        actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        if (!SceneActorChase_Arrive(object)) {
            if (!SceneActorChase_TryRoute(object, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetTypedControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetTypedState(actor, 0, 0, 12);
        return 1;
    }
    if (!body->responseCount) {
        GolemReaction_LatchHitReact(object);
        if (!actor->tracking.target)
            return 0;
        actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        actor->body.direction = actor->tracking.heading;
        group->destination->pad = 1;
        if (!SceneActorChase_Arrive(object)) {
            if (!SceneActorChase_TryRoute(object, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        actor->field2C2 = 0x40;
        if (body->responseCount <= 2)
            body->responseCount++;
        SceneActorChase_SetTypedControl(actor, SceneActorReaction_PlayAnimationBUntilComplete, 10);
        SceneActorChase_SetTypedState(actor, 0, 1, 1);
        SceneActorChase_SetActorFlags(actor, 0x10);
        return 1;
    }

    GolemReaction_LatchHitReact(object);
    if (!actor->tracking.target)
        return 0;
    actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
    SceneActorChase_SetActorFlags(actor, 0x90);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(object)) {
        if (!SceneActorChase_TryRoute(object, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    GolemReaction_Settle(actor);
    return 1;
}

void Golem_UpdateScriptedAction(struct SceneActorRegistryResetView *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorReaction_PlayAnimationBUntilComplete;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->field2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->target = (void *)fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->tracking.heading;
            obj->body.direction = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->category);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->field2B0) {
        Golem_ResetIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}
