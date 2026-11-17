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

/* Bat state callback: restores the saved snapshot (sibling of Vorn_RestoreSavedState)
 * and plays song 0xEE on one animation frame. Owner +0x48 is an animation record.
 * The snapshot fields below are copied and reset at the listed offsets. */

typedef struct Inner {
    u8 pad0[8];
    u32 unk8;
} Inner;

typedef struct Anim {
    u8 pad0[8];
    u16 unk8;
    u8 padA[4];
    u16 unkE;
} Anim;

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    Inner inner;
    u8 pad2C[0x1c];
    Anim anim;
} Owner;

typedef struct Obj {
    u8 pad0[0x4];
    Owner *unk4;
    u8 unk8;
    u8 pad9[0x50];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x38];
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0x48];
    u16 unk178;
    u8 pad17A[0xA2];
    u32 unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x5];
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x8];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
} Obj;

void ActorShadow_Enable(void *);

void Ghoul_ResetRouteState(Obj *);

void Sound_PlaySong(u32);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) {
        s->unk2B2 = 0;
        return 1;
    }
    return 0;
}

/* Keep this byte predicate local; its address is not retained by the body. */

static inline u8 peek_flag(Obj *s)
{
    return s->unk2B3;
}

void Bat_RestoreSavedState(Obj *s)
{
    s32 n;
    if (take_flag(s)) {
        ActorShadow_Enable(s->unk320);
        if (!s->unk8) {
            u32 *q = &s->unk4->unk0;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            Inner *q = &s->unk4->inner;
            u32 mask = ~1;
            q->unk8 = mask & q->unk8;
        }
        {
            u16 one = 1;
            s->unk9C = one;
        }
        s->unk178 = 60;
    }
    {
        Anim *a = &s->unk4->anim;
        if (a->unk8 == 3 && a->unkE == 4)
            Sound_PlaySong(0xEE);
    }
    if (!peek_flag(s))
        return;
    {
        Ghoul_ResetRouteState(s);
        s->unk2CC = s->unk118;
        s->unk2D0 = s->unk11C;
        s->unk59 = s->unk120;
        s->unk2A8 = s->unk121;
        s->unk2A9 = s->unk122;
        s->unk2AA = s->unk123;
        s->unk21C = s->unk124;
        s->unk2B5 = s->unk128;
        s->unk2C0 = s->unk12E;
        s->unk2EA = s->unk12C;
        n = s->unkB0;
        if (n <= 0)
            n = 0;
        s->unk9C = n;
        s->unk2D6 = 0;
        s->unk2D4 = 0;
        s->unk2B0 = 0;
        s->unk2B1 = 1;
        s->unk2B2 = 1;
        s->unk2B3 = 0;
        s->unk304 = 0;
        s->unk2B6 = 0;
        s->unk2C2 = 64;
        s->unk2D8 = 0;
        s->unk2DC = 0;
        s->unk2E0 = 0;
        s->unk2E6 = 0;
        s->unk3CC = 0;
        s->unkDC = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

struct Seq {
    u8 pad0[7];
    u8 b7;
    u16 h8;
    u8 padA[4];
    u16 hE;
};

struct BatAnimationRequest {
    u8 pad0[4];
    u8 *inner;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u8 *q)
{
    ((struct Seq *)q)->hE = 0;
}

void Bat_UpdateAnimationRequest(u8 *p)
{
    struct Seq *s;

    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 5);
    s = (struct Seq *)(*(u8 **)(p + 4) + 0x48);
    if (s->h8 == 7 && s->hE == (s->b7 != 0) && s->b7 != 1) {
        s->h8 = 2;
        Clr(*(u8 **)(p + 4) + 0x48);
    }
}

/* Bat variant of SceneActorReaction_ResumeSavedTarget (tentative): after 60 frames it
 * resumes either the saved route target (kind 1) or, when flag 0x80 is set, group +0x48
 * (kind 0x1A); before that it rewinds a mode-2 animation. */

struct BatRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

struct BatAnim {
    u8 unknown0[7];
    u8 loop;
    u16 mode;
    u8 unknownA[4];
    u16 frame;
};

struct BatDrawable {
    u8 unknown0[0x48];
    struct BatAnim anim;
};

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u32 Bat_MaskFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Bat_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void Bat_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group.common;
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
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x1A, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u16 *flags = (u16 *)(group + 0x40);
                u16 mask = 4;
                *flags = mask | *flags;
            }
        } else {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.onAlternateEntry;
            Bat_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->base.behaviorFlags |= mask;
            }
        }
    } else {
        struct BatAnim *anim = &((struct BatDrawable *)actor->base.drawable)->anim;
        if (anim->mode == 2) {
            u16 f = anim->frame;
            u8 loop = anim->loop;
            if (f == (loop != 0) && loop != 1)
                anim->frame = 0;
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
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
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
        animation = ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
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

static inline void Bat_BeginTurn_UpdateRecoilRecovery(void *obj, u32 saved, const u16 *randomSamples)
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
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
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
        (struct DisplaySequenceState *)((struct SceneActorAnimationSequenceDrawable *)
                                            actor->base.drawable)
            ->sequence;
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
    {
        u32 mask = 0x10000;
        actor->base.statusFlags |= mask;
    }
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

void Ghoul_SelectRetreatRoutePoint(struct ChaseActor *);

static inline u8 TakeEntry(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

struct Bits16 {
    s16 bits : 16;
};

static inline u8 Test(u8 *context, u32 mask, struct Bits16 **out)
{
    struct Bits16 *p = (void *)(context + 0x628);
    u32 value = *(u16 *)p & mask;
    *out = p;
    if (value)
        return 1;
    return 0;
}

static inline void Clear(struct Bits16 *p, s32 mask)
{
    p->bits = mask & p->bits;
}

void Bat_UpdateProbeReaction(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    struct Bits16 *flags;
    if (TakeEntry(a))
        return;
    if (Test(context, 2, &flags)) {
        if (a->base.tracking.distance > 0xFFFF)
            SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Ghoul_SelectRetreatRoutePoint(a);
        Clear(flags, ~2);
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
