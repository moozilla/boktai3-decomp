#include "scene_actor_route.h"
#include "display_keyframe.h"
#include "scene_facing.h"
void ProximityVolume_TranslateBounds(void *, void *);
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
    u8 unk216[0x224 - 0x216];
    u8 probeReady;
    u8 unk225[0x23C - 0x225];
    u8 terrainResponse[0x2E4 - 0x23C];
    s32 (*callback)(struct RouteWalkActor *);
};

static inline s32 RouteWalk_Scale12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void RouteWalk_Polar(struct ContactPosition *step, s32 angle)
{
    s32 speed = 48;
    step->x = RouteWalk_Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    step->y = 0;
    step->z = RouteWalk_Scale12(speed * gUnk_086149C4[angle]);
}

// confidence=low: walks toward the navigation target while the animation permits a fixed motion step.
#define SceneActorAnimation_UpdateNavigationStep sub_0802F760
void SceneActorAnimation_UpdateNavigationStep(void *unused, struct RouteWalkActor *p)
{
    struct RouteWalkMotion *motion = &p->motion;
    struct RouteWalkProbe *probe = &p->probe;
    u32 *animType = &p->animType;
    if (p->entered) {
        s32 zero = 0;
        struct ContactPosition *pos;
        p->entered = zero;
        p->finished = zero;
        Actor_PlayAnimationEntry(p, 1, 1, zero, 4);
        p->routeProgress = zero;
        pos = &p->motion.pos;
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, p->terrainResponse);
        p->probeReady = 1;
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
                struct ContactPosition step;
                p->motion.angle = angle;
                p->facing8 = SceneFacing_EightWay(angle);
                p->facing4 = SceneFacing_FourWay(angle);
                RouteWalk_Polar(&step, motion->angle);
                motion->pos.x += step.x;
                motion->pos.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, current);
        Actor_PlayFootstepSound(p, 1);
        Actor_PlayAnimationEntry(p, 1, 1, 0, 1);
    }
}
