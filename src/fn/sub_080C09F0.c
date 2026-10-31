#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH       1
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
void Skeleton_UpdateScriptWaitState(void);
s32 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
struct SkeletonApproachProbe {
    u8 flags;
};

struct SkeletonChaseBody {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 minimumDistance, maximumDistance;
};
static inline s32 Skeleton_ScaleApproachMotion(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}
static inline void Skeleton_SetApproachState(void *object, u8 state, u8 phase, u32 behavior)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Skeleton_GetApproachCellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    position->x = ((u32)x << 8) + 0x80;
    {
        u32 attributes = SceneActorChase_CellProperties(cell)->attributes;
        u32 height = 15;
        height &= attributes;
        position->y = height << 8;
    }
    position->z = ((u32)z << 8) + 0x80;
}

static inline u8 Skeleton_ApproachTargetCell(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t || (player = SceneActorChase_PlayerSlot(t)) < 0) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
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
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    sub_0807E334(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    Skeleton_GetApproachCellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

void Skeleton_UpdateTargetApproach(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonChaseBody *body = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        void *target = Skeleton_UpdateScriptWaitState;
        SceneActorChase_SetControl(actor, target, 0x26);
    }
    if (((struct SkeletonApproachProbe *)actor->body.movementProbe)->flags & 1) {
        Skeleton_SetApproachState(actor, 0, 0, 12);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Skeleton_ApproachTargetCell(actor);
        {
            struct SkeletonChaseBody *b = actor->context;
            u32 mask = 0x80;
            b->flags |= mask;
        }
    } else {
        struct ContactPosition diff;
        u32 angle;
        s32 speed;
        {
            struct ContactPosition *out = &diff;
            struct ContactPosition *from = &actor->body.position;
            struct ContactPosition *to = &actor->tracking.target->position;
            SceneActorTerrain_Subtract(out, to, from);
        }
        angle = Math_Atan2(diff.x, diff.z);
        if (actor->tracking.distance < body->minimumDistance) {
            angle = (angle + 0x80) & 0xFF;
            actor->field2C2 = 0x20;
        } else if (actor->tracking.distance > body->maximumDistance) {
            actor->field2C2 = 0x20;
            actor->body.direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
        {
            u32 mask = 0xA02F;
            u16 *scale;
            if (actor->flags2D4 & mask) {
                actor->field2C2 = 0x40;
                scale = &actor->field2C2;
            } else {
                u16 t = actor->field2C2;
                scale = &actor->field2C2;
                if (t == 0)
                    *scale = 0x40;
            }
            if (actor->drawableKind == 0)
                Anim_SetDurationScale(
                    ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence,
                    *scale);
            else
                Anim_SetFrameDurationScale(
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
        }
        actor->body.motion.x +=
            Skeleton_ScaleApproachMotion(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
        actor->body.motion.z += Skeleton_ScaleApproachMotion(gUnk_086149C4[angle & 0xFF] * speed);
        {
            u32 mask = 0x2000;
            actor->flags2CC |= mask;
        }
        if (actor->tracking.distance > body->minimumDistance &&
            actor->tracking.distance < body->maximumDistance) {
            u16 mask = 0x80;
            actor->flags2D6 = mask | actor->flags2D6;
        } else
            actor->counter2AC++;
    }
}
