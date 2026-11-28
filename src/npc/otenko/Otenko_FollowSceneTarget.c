#include "global.h"
#include "npc/otenko/otenko_actor.h"
#include "engine/math/math_shift.h"
#include "scene/motion/scene_facing.h"
#include "monsters/ghoul_disc.h"
#include "ui/hud/scene_indicator.h"

struct FollowTarget {
    u8 unk_00[0x2D];
    u8 angle;
    u8 unk_2E[2];
    struct ContactPosition position;
};
struct Context {
    u8 unk_00[0x434];
    struct ContactPosition *position;
    struct FollowTarget *target;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline void Polar(struct ContactPosition *out, s32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(speed * gUnk_086149C4[(angle + 64) & 255], 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(speed * gUnk_086149C4[angle], 12);
}
static inline void Face(struct OtenkoActor *npc, s32 angle)
{
    npc->body.direction = angle;
    npc->facing8 = SceneFacing_EightWay(angle);
    npc->facing4 = SceneFacing_FourWay(angle);
}

/* Tentative Otenko_FollowSceneTarget: follow the manager's selected actor,
 * snapping behind it when separated and permitting conversation when nearby. */
void Otenko_FollowSceneTarget(struct Context *ctx, struct OtenkoActor *npc)
{
    struct FollowTarget *target = ctx->target;
    struct SceneActorBody *motion = &npc->body;
    struct ContactPosition *position = &motion->position;
    struct ContactPosition offset;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 0, 1, 0, 4);
    }
    {
        s32 mask = 0x402;
        npc->displayFlags = mask | npc->displayFlags;
    }
    if ((u8)Callback_InvokeActor(npc, npc->animationCallback) == 1)
        return;
    if (target) {
        s32 targetAngle = target->angle;
        struct ContactPosition *targetPosition = &target->position;
        s32 angle =
            Math_Atan2(targetPosition->x - motion->position.x, targetPosition->z - position->z);
        s32 dx = targetPosition->x - motion->position.x;
        s32 dz = targetPosition->z - position->z;
        s32 distance = dx * dx + dz * dz;
        s32 snapDistance = 0x90000;
        struct ContactPosition *snapPosition = position;
        /* Snap to a point three world units behind a distant target. */
        if (distance > snapDistance) {
            Face(npc, angle);
            Polar(&offset, motion->direction, -768);
            {
                struct ContactPosition *offsetPosition = &offset;
                struct ContactPosition *targetPosition = &target->position;
                motion->position.x = targetPosition->x + offsetPosition->x;
                snapPosition->y = targetPosition->y + offsetPosition->y;
                snapPosition->z = targetPosition->z + offsetPosition->z;
            }
        } else if (distance > 0xFFFF) {
            Face(npc, angle);
            Polar(&motion->motion, motion->direction, 24);
        } else {
            Face(npc, angle);
            if (Ghoul_Abs(motion->position.y - ctx->position->y) <= 255) {
                struct ContactPosition *destination = ctx->position;
                s32 direction =
                    (s8)Math_Atan2(position->x - destination->x, position->z - destination->z);
                s32 delta = targetAngle - direction;
                s32 oppositeFacing = npc->body.direction + 128;
                u8 opposite = targetAngle - oppositeFacing;
                if (Ghoul_Abs((s8)delta) <= 63 && Ghoul_Abs((s8)opposite) <= 31) {
                    s32 mask = 0x800;
                    npc->displayFlags = mask | npc->displayFlags;
                }
            }
        }
    }
    Actor_PlayAnimationEntry(npc, 0, 1, 0, 1);
}
