#include "scene_actor_route.h"
#include "display_keyframe.h"
void ProximityVolume_TranslateBounds(void *, void *);
u32 Route_Start(struct SceneActorRouteCursor *, u32, u32, u32);
void Path_GetPointPosition(struct ContactPosition *, struct SceneActorRoutePoint *, u32);
s32 Terrain_ApplyBlocked(void *, void *, void *);
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Math_Atan2(s32, s32);
void NavMesh_UpdateProbe(void *, void *);
void Actor_PlayFootstepSound(void *, s32);

struct RouteWalkMotion {
    u8 unk00[9];
    u8 angle;
    u8 unk0A[2];
    struct ContactPosition pos;
};

struct RouteWalkProbe {
    u8 unk00[0xC];
    struct ContactPosition current;
    u8 unk14[8];
    struct ContactPosition target;
};

struct RouteWalkActor {
    u8 unk00[7];
    u8 finished;
    u8 unk08;
    u8 entered;
    u8 unk0A[4];
    u16 flags;
    u8 unk10[9];
    u8 facing4;
    u8 facing8;
    u8 unk1B[0x5C - 0x1B];
    struct RouteWalkMotion motion;
    u8 unk70[0xD4 - 0x70];
    u32 animType;
    u8 unkD8[0x100 - 0xD8];
    struct DisplayKeyframeState keyframe;
    u8 unk13C[0x19C - 0x100 - sizeof(struct DisplayKeyframeState)];
    struct SceneActorRouteCursor route;
    struct RouteWalkProbe probe;
    u8 unk1CC[0x1F0 - 0x1CC];
    u8 routeProgress;
    u8 unk1F1[2];
    s8 routeId;
    u8 unk1F4[0x210 - 0x1F4];
    u16 routeMode;
    u8 unk212[0x214 - 0x212];
    u16 routePoint;
    u8 unk216[0x2E4 - 0x216];
    s32 (*callback)(struct RouteWalkActor *);
};

static inline s32 RouteWalk_Scale12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void RouteWalk_Polar(struct ContactPosition *step, s32 angle, s32 speed)
{
    step->x = RouteWalk_Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    step->y = 0;
    step->z = RouteWalk_Scale12(speed * gUnk_086149C4[angle]);
}
static inline void RouteWalk_SetFacing(struct RouteWalkActor *p, u16 angle)
{
    s32 value = angle;
    s32 shifted = value + 112;
    s32 facing8;
    s32 facing4;
    shifted >>= 5;
    facing8 = 7;
    facing8 &= shifted;
    p->facing8 = facing8;
    value = (value + 96) >> 6;
    facing4 = 3;
    facing4 &= value;
    p->facing4 = facing4;
}
void SceneActorAnimation_UpdateRouteWalk(void *unused, struct RouteWalkActor *p)
{
    struct RouteWalkMotion *motion = &p->motion;
    struct RouteWalkProbe *probe = &p->probe;
    struct SceneActorRouteCursor *route = &p->route;
    u32 *animType = &p->animType;
    if (p->entered) {
        struct ContactPosition path;
        struct ContactPosition *pos;
        struct ContactPosition *target;
        s32 angle;
        p->entered = 0;
        p->finished = 0;
        p->routeProgress = 0;
        pos = &p->motion.pos;
        ProximityVolume_TranslateBounds(probe, pos);
        Route_Start(route, p->routeId, p->routeMode, p->routePoint);
        Path_GetPointPosition(&path, p->route.point, 0);
        Terrain_ApplyBlocked(probe, pos, &path);
        target = &p->probe.target;
        angle = Math_Atan2(probe->target.x - motion->pos.x, target->z - pos->z);
        p->motion.angle = angle;
        RouteWalk_SetFacing(p, angle);
        Actor_PlayAnimationEntry(p, 1, 1, 0, 4);
    }
    {
        u32 mask = 0x1000;
        p->flags |= mask;
    }
    if ((u8)p->callback(p) != 1) {
        struct ContactPosition *pos = &p->motion.pos;
        struct ContactPosition *probePos = &p->probe.current;
        s32 x = probe->current.x - motion->pos.x;
        s32 z = probePos->z - pos->z;
        s32 distance = (s32)((u32)x * x + (u32)z * z);
        s32 limit = 0x9C3;
        struct ContactPosition *current = pos;
        if (distance <= limit) {
            motion->pos.x = p->probe.target.x;
            motion->pos.z = p->probe.target.z;
            p->finished = 1;
        } else if (*animType == 2) {
            struct DisplayKeyframeState *anim = &p->keyframe;
            if (anim->status & 4) {
                struct ContactPosition *target = &p->probe.target;
                s32 angle = Math_Atan2(probe->target.x - motion->pos.x, target->z - current->z);
                s32 speed;
                struct ContactPosition step;
                p->motion.angle = angle;
                RouteWalk_SetFacing(p, angle);
                speed = (anim->frame & 1) ? 24 : 48;
                RouteWalk_Polar(&step, motion->angle, speed);
                motion->pos.x += step.x;
                motion->pos.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, current);
        Actor_PlayFootstepSound(p, 1);
        Actor_PlayAnimationEntry(p, 1, 1, 0, 1);
    }
}
