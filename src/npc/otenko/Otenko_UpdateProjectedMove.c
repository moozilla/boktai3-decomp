#include "global.h"
#include "npc/otenko/otenko_actor.h"
#include "scene/motion/scene_facing.h"
#include "scene/collision/packed_contact_vector.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
s32 Player_GetHorizontalVelocity(s32);
static inline s32 scale8(s32 n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}
static inline s32 interpolate(s32 start, s32 end, s32 duration, s32 time)
{
    if (time <= 0)
        return start;
    if (time >= duration)
        return end;
    return start + scale8(((end - start) << 8) * time / duration);
}
static inline void facing(struct OtenkoActor *npc, u32 angle)
{
    npc->body.direction = angle;
    npc->facing8 = SceneFacing_EightWay(angle);
    npc->facing4 = SceneFacing_FourWay(angle);
}

static inline void project(struct OtenkoActor *npc, struct ContactPosition *position)
{
    struct OtenkoProjection *projection = &npc->projection;
    projection->position.x = (position->x + gSceneCamera->x) << 12;
    projection->position.z = position->z << 12;
    projection->position.y = 0;
    projection->height = position->y << 12;
}
static inline void projectTarget(struct OtenkoActor *npc)
{
    struct OtenkoProjection *projection = &npc->projection;
    struct ContactPosition *position = &npc->target;
    projection->position.x = (position->x + gSceneCamera->x) << 12;
    projection->position.z = position->z << 12;
    projection->position.y = 0;
    projection->height = position->y << 12;
}
void Otenko_UpdateProjectedMove(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    struct OtenkoEffectState *effect = &npc->effect;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        GroundEffect_EnableProjection(&npc->groundEffect);
        Actor_PlayAnimationEntry(npc, 3, 1, 0, 4);
        npc->animationPhase = 0;
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        switch (npc->animationPhase) {
            case 0:
                if (tick >= effect->dashDuration) {
                    projectTarget(npc);
                    npc->animationPhase++;
                } else {
                    struct PackedContactVector position;
                    position.x = interpolate(npc->savedPosition.x, npc->target.x,
                                             effect->dashDuration, tick);
                    position.y = interpolate(npc->savedPosition.y, npc->target.y,
                                             effect->dashDuration, tick);
                    position.z = interpolate(npc->savedPosition.z, npc->target.z,
                                             effect->dashDuration, tick);
                    facing(npc, npc->targetAngle);
                    Actor_PlayAnimationEntry(npc, 3, 1, 0, 4);
                    project(npc, (struct ContactPosition *)&position);
                }
                break;
            case 1:
                npc->projection.position.x += Player_GetHorizontalVelocity(0);
                npc->done = 1;
                break;
        }
    }
}
