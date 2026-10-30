#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_animation.h"

extern const s16 gUnk_086149C4[];
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void sub_080A9DB0(void *, s32, s32);

struct TentacleReturnContext {
    u8 unknown0[0x87A];
    u16 startCell;
    u8 unknown87C[0x924 - 0x87C];
    struct ContactPosition center;
};

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

static inline void Tentacle_SubtractPositions(struct ContactPosition *out,
                                              const struct ContactPosition *from,
                                              const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void Tentacle_BeginForwardState(struct SceneActorRegistryResetView *a)
{
    u32 behavior = 2;
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

static inline void Tentacle_ApplyDurationScale(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
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

void TentacleBody_ReturnToCellCenter(struct SceneActorRegistryResetView *actor)
{
    struct TentacleReturnContext *body = actor->context;
    s32 speed;
    s32 difference;
    s32 index;
    struct ContactPosition target;
    if (Tentacle_TakeEntry(&actor->field2B1)) {
        {
            u32 cell = actor->terrainCell;
            body->startCell = cell;
        }
        SceneActorTerrain_GetCenter(body->startCell, &body->center);
    }
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    Tentacle_ApplyDurationScale(actor);
    difference = Tentacle_HeadingDifference(actor->unknown3CD[0], actor->body.direction);
    {
        s32 magnitude = (difference < 0) ? (-difference) : (difference);
        if (magnitude > 8) {
            s32 x = Tentacle_ShiftTowardZero(
                speed * gUnk_086149C4[(actor->body.direction + 64) & 255], 12);
            s32 z = Tentacle_ShiftTowardZero(speed * gUnk_086149C4[actor->body.direction], 12);
            actor->body.motion.x += x;
            actor->body.motion.z += z;
        }
    }
    Tentacle_AddPositions(&target, &actor->body.motion, &actor->body.position);
    index = SceneActorTerrain_GetCellIndex(&target);
    if ((body->startCell) != index) {
        Tentacle_SubtractPositions(&target, &target, &body->center);
        actor->body.motion.x += Div(target.x, 10);
        actor->body.motion.z += Div(target.z, 10);
    }
    sub_080A9DB0(actor, 12, 0xA410);
    if (((difference < 0) ? (-difference) : (difference)) <= 7) {
        if ((body->startCell) == index)
            Tentacle_BeginForwardState(actor);
    } else if (difference > 0) {
        actor->body.direction += (((64 - actor->field2C2) >> 4) << 1) + 1;
    } else {
        actor->body.direction -= (((64 - actor->field2C2) >> 4) << 1) + 1;
    }
    actor->counter2AC++;
}
