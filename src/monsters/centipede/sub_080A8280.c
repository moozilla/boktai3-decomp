#include "monsters/centipede/centipede_body.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_terrain.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/motion/scene_actor_chase.h"

extern const s16 gUnk_086149C4[];
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Centipede_UpdateSegmentPositions(void *, s32, s32);

static inline u8 Tentacle_TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline s32 Tentacle_ShiftTowardZero(s32 n, u32 bits)
{
    s32 result;
    if (n >= 0)
        result = n >> bits;
    else
        result = -((-n) >> bits);
    return result;
}

static inline void Tentacle_BeginForwardState(struct ChaseActor *a)
{
    u32 behavior = 2;
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

static inline void Tentacle_ApplyDurationScale(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

static inline void Tentacle_AddPositions(struct ContactPosition *out,
                                         const struct ContactPosition *a,
                                         const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline s32 Tentacle_HeadingDifference(u8 desired, u8 current)
{
    s32 difference = desired - current;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    return difference;
}

void CentipedeBody_ReturnToCellCenter(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    s32 speed;
    s32 difference;
    s32 index;
    struct ContactPosition target;
    if (Tentacle_TakeEntry(&actor->base.field2B1)) {
        {
            u32 cell = actor->base.terrainCell;
            body->returnCell = cell;
        }
        SceneActorTerrain_GetCenter(body->returnCell, &body->returnCenter);
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Tentacle_ApplyDurationScale(actor);
    difference = Tentacle_HeadingDifference(actor->base.nextDirection, actor->base.body.direction);
    {
        s32 magnitude = (difference < 0) ? (-difference) : (difference);
        if (magnitude > 8) {
            s32 x = Tentacle_ShiftTowardZero(
                speed * gUnk_086149C4[(actor->base.body.direction + 64) & 255], 12);
            s32 z = Tentacle_ShiftTowardZero(speed * gUnk_086149C4[actor->base.body.direction], 12);
            actor->base.body.motion.x += x;
            actor->base.body.motion.z += z;
        }
    }
    Tentacle_AddPositions(&target, &actor->base.body.motion, &actor->base.body.position);
    index = SceneActorTerrain_GetCellIndex(&target);
    if ((body->returnCell) != index) {
        Chase_Delta(&target, &target, &body->returnCenter);
        actor->base.body.motion.x += Div(target.x, 10);
        actor->base.body.motion.z += Div(target.z, 10);
    }
    Centipede_UpdateSegmentPositions(actor, 12, 0xA410);
    if (((difference < 0) ? (-difference) : (difference)) <= 7) {
        if ((body->returnCell) == index)
            Tentacle_BeginForwardState(actor);
    } else if (difference > 0) {
        actor->base.body.direction += (((64 - actor->base.field2C2) >> 4) << 1) + 1;
    } else {
        actor->base.body.direction -= (((64 - actor->base.field2C2) >> 4) << 1) + 1;
    }
    actor->base.counter2AC++;
}
