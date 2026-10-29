#include "global.h"

struct Vec {
    s16 x, y, z, pad;
};
struct Motion {
    u8 unk_00[9];
    u8 angle;
    u8 unk_0A[2];
    struct Vec position;
    struct Vec velocity;
};
struct SceneNpc {
    u8 unk_00[7];
    u8 done;
    u8 unk_08;
    u8 entered;
    u8 unk_0A[4];
    u16 flags;
    u8 unk_10[9];
    u8 facing4;
    u8 facing8;
    u8 unk_1B[0x41];
    struct Motion motion;
    u8 unk_78[0x26C];
    void *callback;
};
struct FollowTarget {
    u8 unk_00[0x2D];
    u8 angle;
    u8 unk_2E[2];
    struct Vec position;
};
struct Context {
    u8 unk_00[0x434];
    struct Vec *position;
    struct FollowTarget *target;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
static inline s32 Abs(s32 x)
{
    if (x < 0)
        return -x;
    return x;
}
static inline s32 Scale(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void Polar(struct Vec *out, s32 angle, s32 speed)
{
    out->x = Scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Scale(speed * gUnk_086149C4[angle]);
}
static inline void Face(struct SceneNpc *npc, s32 angle)
{
    u16 a;
    npc->motion.angle = angle;
    a = angle;
    {
        s32 t = a + 112;
        u32 mask;
        t >>= 5;
        mask = 7;
        mask &= t;
        npc->facing8 = mask;
    }
    {
        s32 t = (a + 96) >> 6;
        u32 mask = 3;
        mask &= t;
        npc->facing4 = mask;
    }
}

/* Tentative SceneNpc_FollowSceneTarget: follow the manager's selected actor,
 * snapping behind it when separated and permitting conversation when nearby. */
void SceneNpc_FollowSceneTarget(struct Context *ctx, struct SceneNpc *npc)
{
    struct FollowTarget *target = ctx->target;
    struct Motion *motion = &npc->motion;
    struct Vec *position = &motion->position;
    struct Vec offset;
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 0, 1, 0, 4);
    }
    {
        s32 mask = 0x402;
        npc->flags = mask | npc->flags;
    }
    if ((u8)Callback_InvokeActor(npc, npc->callback) == 1)
        return;
    if (target) {
        s32 targetAngle = target->angle;
        struct Vec *targetPosition = &target->position;
        s32 angle =
            Math_Atan2(targetPosition->x - motion->position.x, targetPosition->z - position->z);
        s32 dx = targetPosition->x - motion->position.x;
        s32 dz = targetPosition->z - position->z;
        s32 distance = dx * dx + dz * dz;
        s32 snapDistance = 0x90000;
        struct Vec *snapPosition = position;
        /* Snap to a point three world units behind a distant target. */
        if (distance > snapDistance) {
            Face(npc, angle);
            Polar(&offset, motion->angle, -768);
            {
                struct Vec *offsetPosition = &offset;
                struct Vec *targetPosition = &target->position;
                motion->position.x = targetPosition->x + offsetPosition->x;
                snapPosition->y = targetPosition->y + offsetPosition->y;
                snapPosition->z = targetPosition->z + offsetPosition->z;
            }
        } else if (distance > 0xFFFF) {
            Face(npc, angle);
            Polar(&motion->velocity, motion->angle, 24);
        } else {
            Face(npc, angle);
            if (Abs(motion->position.y - ctx->position->y) <= 255) {
                struct Vec *destination = ctx->position;
                s32 direction =
                    (s8)Math_Atan2(position->x - destination->x, position->z - destination->z);
                s32 delta = targetAngle - direction;
                s32 oppositeFacing = npc->motion.angle + 128;
                u8 opposite = targetAngle - oppositeFacing;
                if (Abs((s8)delta) <= 63 && Abs((s8)opposite) <= 31) {
                    s32 mask = 0x800;
                    npc->flags = mask | npc->flags;
                }
            }
        }
    }
    Actor_PlayAnimationEntry(npc, 0, 1, 0, 1);
}
