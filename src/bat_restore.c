#include "global.h"
#include "player.h"
#include "scene_actor_reaction.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_delayed_response.h"
#include "display_sequence.h"
#include "scene_actor_displacement.h"
#include "spark_particle.h"
#include "scene_actor_chase.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "engine.h"
#include "scene_actor_part_burst.h"
#include "ghoul.h"
#include "scene_actor_init_actor.h"

void ActorShadow_Enable(void *);

void Ghoul_ResetRouteState(struct ChaseActor *);

void Sound_PlaySong(u32);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 Bat_IsRevivalReady(struct ChaseActor *actor)
{
    return actor->base.restart;
}

void Bat_RestoreSavedState(struct ChaseActor *actor)
{
    s32 health;
    if (SceneActorReaction_TakePrepared(actor)) {
        ActorShadow_Enable(actor->base.shadow);
        if (!actor->base.drawableKind) {
            u32 *q =
                &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sprite.flags;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            struct SceneInputWidget *q =
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
            u32 mask = ~1;
            q->flags = mask & q->flags;
        }
        SceneActorReaction_BeginRevival(&actor->base, 1);
    }
    {
        struct DisplaySequenceState *a =
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (a->index == 3 && a->elapsed == 4)
            Sound_PlaySong(0xEE);
    }
    if (!Bat_IsRevivalReady(actor))
        return;
    {
        Ghoul_ResetRouteState(actor);
        actor->base.behaviorFlags = actor->base.saved.behaviorFlags;
        actor->base.statusFlags = actor->base.saved.statusFlags;
        actor->base.body.direction = actor->base.saved.direction;
        actor->base.state2A8 = actor->base.saved.state;
        actor->base.phase2A9 = actor->base.saved.phase;
        actor->base.behavior2AA = actor->base.saved.behavior;
        actor->base.target = (void *)actor->base.saved.control;
        actor->base.controlKind = actor->base.saved.kind;
        actor->base.placementFlags = actor->base.saved.motionFlags;
        actor->base.heightOffset = actor->base.saved.heightOffset;
        health = actor->base.defaultHealth;
        if (health <= 0)
            health = 0;
        actor->base.health = health;
        actor->base.reactionFlags = 0;
        actor->base.motionFlags = 0;
        actor->base.stepDone = 0;
        actor->base.field2B1 = 1;
        actor->base.field2B2 = 1;
        actor->base.restart = 0;
        actor->base.counter304 = 0;
        actor->base.reactionDirection = 0;
        actor->base.field2C2 = 64;
        actor->base.hitAttributes = 0;
        actor->base.hitReactionFlags = 0;
        actor->base.hitOwners = 0;
        actor->base.field2E6 = 0;
        actor->base.field3CC = 0;
        actor->base.tracking.target = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

static inline void Bat_RewindAnimation(struct DisplaySequenceState *sequence)
{
    sequence->elapsed = 0;
}

void Bat_UpdateAnimationRequest(struct ChaseActor *actor)
{
    struct DisplaySequenceState *sequence;

    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 5);
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (sequence->index == 7 && sequence->elapsed == (sequence->duration != 0) &&
        sequence->duration != 1) {
        sequence->index = 2;
        Bat_RewindAnimation(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence);
    }
}

/* Bat variant of SceneActorReaction_ResumeSavedTarget (tentative): after 60 frames it
 * resumes either the saved route target (kind 1) or, when flag 0x80 is set, group +0x48
 * (kind 0x1A); before that it rewinds a mode-2 animation. */

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u32 Bat_MaskFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

void Bat_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        actor->base.restart = 0;
    }
    if (frame > 60) {
        u16 blocked;
        if (!actor->base.restart)
            return;
        actor->base.stepDone = 1;
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     ((struct ChaseRouteProbe *)group->route)->destination.x,
                                     ((struct ChaseRouteProbe *)group->route)->destination.z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = group->common.gate.control;
            SceneActorChase_SetControl(actor, target, 0x1A);
            SceneActorChase_SetState(actor, 0, 0, 12);
            SceneActorChase_SetFlagComponent(
                (struct SceneActorChaseFlags16 *)&group->common.gate.flags, 4);
        } else {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     ((struct ChaseRouteProbe *)group->route)->destination.x,
                                     ((struct ChaseRouteProbe *)group->route)->destination.z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.onAlternateEntry;
            SceneActorChase_SetControl(actor, target, 1);
            SceneActorChase_SetState(actor, 0, 0, 7);
            SceneActorChase_SetActorFlags(actor, 16);
        }
    } else {
        struct DisplaySequenceState *anim =
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (anim->index == 2) {
            u16 f = anim->elapsed;
            u8 loop = anim->duration;
            if (f == (loop != 0) && loop != 1)
                anim->elapsed = 0;
        }
    }
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline struct SceneActorChaseGroup *Bat_GroupOf(struct ChaseActor *actor)
{
    return &actor->group;
}

static inline void Bat_BeginTurn(void *obj, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Bat_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Bat_Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = Bat_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&(t)->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_GroupClearFlags(group, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Bat_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline u16 *Bat_PrepareLanding(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *animation;
    u16 *reactionFlags;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    SceneActorChase_SetReactionFlags(actor, 0x10);
    reactionFlags = &actor->base.reactionFlags;
    if (frame <= 17) {
        animation = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
            animation->duration != 1)
            animation->elapsed = 0;
    }
    return reactionFlags;
}

/* Resume the suspended landing reaction, then queue pursuit or the home route. */

void Bat_UpdateLandingRecovery(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u16 *reactionFlags;
    reactionFlags = Bat_PrepareLanding(actor, frame);
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    if (SceneActorChase_QueryMotionFlagComponent(actor, 2)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, group->contactControl, 7);
        SceneActorChase_ClearBits(reactionFlags, 0x983);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        Bat_Arrive(actor);
        return;
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 60);
    actor->base.field2C2 = 64;
    SceneActorChase_SetActorFlags(actor, 0x10);
    SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                  group->destination)) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
        SceneActorChase_SetState(actor, 3, 0, 0);
        return;
    }
    group->destination->pad = 1;
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline void Bat_SaveControl(struct SceneActorChaseSavedControl *packet, void *object)
{
    struct ChaseActor *actor = object;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}

static inline s32 Bat_MapWidth(struct EngineMap *map)
{
    return ((struct SceneActorChaseTerrainGrid *)map->cells)->width;
}

static inline void Bat_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, Bat_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, Bat_MapWidth(SceneActorChase_CurrentMap()));
    position->x = (s16)(((u32)x << 8) + 0x80);
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = SceneActorChase_TerrainLevelHeight(height);
    }
    position->z = (s16)(((u32)z << 8) + 0x80);
}

static inline u8 Bat_CoordinateFraction(s16 coordinate)
{
    return coordinate;
}

static inline u32 Bat_HeightAt(s32 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainCell *data = SceneActorChase_CellProperties(cell);
    u32 kind, height;
    kind = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= Bat_CoordinateFraction(position->z);
            break;
        case 2:
            height -= Bat_CoordinateFraction(position->x);
            break;
    }
    return height;
}

static inline u32 Bat_Height(struct ContactPosition *position)
{
    return Bat_HeightAt(SceneActorChase_CellAt(position), position);
}

static inline void Bat_SnapToRoute(struct ContactPosition *position,
                                   struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Bat_Height(position);
}

static inline void Bat_BeginTurn_UpdateRecoilRecovery(void *obj, u32 saved,
                                                      const u16 *randomSamples)
{
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    if (!SceneActorChase_MotionFlags(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        Bat_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 6;
    }
}

static inline u8 Bat_Arrive_UpdateRecoilRecovery(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Bat_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&(t)->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_GroupClearFlags(group, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Bat_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            Bat_HeightAt(SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Bat_BeginTurn_UpdateRecoilRecovery(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    Bat_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

void Bat_UpdateLandingRecovery(struct ChaseActor *, s32);

static inline u8 Bat_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

/* Ease recoil displacement, then select route or pursuit recovery. */

void Bat_UpdateRecoilRecovery(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *animation =
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    struct ContactPackedVector range;
    struct SceneActorChaseGroup *group = &actor->group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(&actor->base, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0xD4);
            Sound_PlaySong(0x12A);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->base.restart = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    MonsterInit_SetStatusFlags(actor, 0x10000);
    if (frame <= 17 && animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
        animation->duration != 1)
        animation->elapsed = 0;
    if (Mod(frame, 12) == 0 && SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        u32 index, sample;
        u8 count;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        sample &= 3;
        count = sample + 1;
        SparkParticle_Spawn(&actor->base.body.position, count,
                            (const struct ContactPosition *)&range,
                            actor->base.body.direction + 128, 10, 6, 2, 2);
    }
    dx = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                         &x);
    dz = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                         &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Bat_DisplacementStopped(dx, dz))
        return;
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->base.hitReactionFlags & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Bat_UpdateLandingRecovery, 0x14);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->base.field2C2 = 64;
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_TrackPlayer(actor);
        if (!Bat_Arrive_UpdateRecoilRecovery(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return;
    }
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!Bat_Arrive_UpdateRecoilRecovery(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->base.field2C2 = 32;
}

// confidence=low: reacts to the context probe flag and enters behavior 15 after clearing it.

static inline u8 Bat_TestProbeFlags(struct Ghoul *context, u32 mask,
                                    struct SceneActorChaseFlags16 **out)
{
    struct SceneActorChaseFlags16 *p = (void *)&context->discFlags;
    u32 value = p->value & mask;
    *out = p;
    if (value)
        return 1;
    return 0;
}

void Bat_UpdateProbeReaction(struct ChaseActor *a)
{
    struct Ghoul *context = a->base.context;
    struct SceneActorChaseFlags16 *flags;
    if (SceneActorReaction_TakePrepared(a))
        return;
    if (Bat_TestProbeFlags(context, 2, &flags)) {
        if (a->base.tracking.distance > 0xFFFF)
            SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Ghoul_SelectRetreatRoutePoint(a);
        SceneActorChase_ClearFlagComponent(flags, 2);
        Chase_SetBehavior(a, 15);
    } else
        SceneActorAnimation_SelectDirectionalEntry(a, 1);
}

/* Bat defeat state (tentative; compare SceneActorReaction_BeginHiddenDefeat): on entry it
 * releases attachments, shows the drawable and clears the route; every 12 frames it emits
 * sparks while flag 8 is set, then a six-spark burst, the defeat script, the event flag and
 * the switch to SceneActorControl_OnIdle. */

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorEffects_AdvanceSteps(void *, u32);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

struct BatDrawableUpdateDefeat {
    u32 flags;
    u8 unknown4[4];
    u8 alpha8, alpha9;
    u8 unknownA[0x3E];
    u8 anim[8];
    u16 mode;
    u8 unknown52[4];
    u16 frame;
};

static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}

static inline u32 Random4(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)gRandomTable + index);
    v &= 3;
    return v + 1;
}

union HitSparkTable {
    const u16 *table;
    struct {
        u16 offset;
        u8 page;
        u8 region;
    } encoded;
};

static inline const u16 *ReadSparkTable(const union HitSparkTable *r)
{
    return r->table;
}

void Bat_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct BatDrawableUpdateDefeat *drawable = actor->base.drawable;
    u8 *anim = drawable->anim;
    u16 script;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        drawable->flags |= 2;
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Flag(actor, 8, 0x2D4))
            Sound_PlaySong(0x12A);
        {
            struct SceneActorRegistryControl *control = &actor->group.common.control;
            u32 *field = &actor->base.behaviorFlags;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            collider->flags |= 4;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (Mod(frame, 12) == 0 && Flag(actor, 8, 0x2D4)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->base.body.position, Random4(), &rangeA,
                            actor->base.body.direction + 128, 10, 8, 2, 2);
    }
    if (frame <= 0x22)
        *(u16 *)(anim + 0xE) = 0;
    if (*(u16 *)(anim + 8) <= 2) {
        drawable->alpha8 = 0x46;
        drawable->alpha9 = 0x46;
    } else {
        drawable->alpha8 = 0x40;
        drawable->alpha9 = 0x40;
    }
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    if (!Flag(actor, 0x400, 0x2CC)) {
        u8 category;
        SceneActorBehavior_CallTerrainScript(actor);
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            union HitSparkTable source = {0};
            const u16 *table;
            u8 firstCount;
            source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
            source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
            source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
            table = ReadSparkTable(&source);
            firstCount = SceneActorEffects_NextSparkCount(table);
            SceneActorEffects_SpawnDefeatSparks(actor, (const struct ContactPosition *)&rangeB,
                                                table, firstCount);
        }
        category = actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    script = (u16)actor->base.script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.parameters1FC[0];
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        values[6] = (s32)actor->base.hitOwners;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        u16 *flags;
        u16 masked;
        u32 test = actor->base.motionFlags & mask;
        flags = &actor->base.motionFlags;
        if (!test && (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        actor->base.attachmentTimer = 0;
        drawable->alpha8 = 0x40;
        drawable->alpha9 = 0x40;
        drawable->flags = (drawable->flags & ~2) | 1;
        {
            u32 mask2 = 0x4000;
            u32 value = *flags;
            masked = value & mask2;
        }
        if (!masked) {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            actor->base.field2B2 = 1;
            actor->base.stepDone = 0;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = masked;
            {
                u32 *field = &actor->base.pendingControlFlags;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->base.pendingControl = masked;
            {
                u32 behavior = 2;
                actor->base.state2A8 = 1;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = masked;
                actor->base.field2B1 = 1;
            }
        }
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
}
