#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct Tag {
    u8 pad[0x12];
    u16 duration;
};
struct Projection {
    u8 pad[0x10];
    s32 x, y, z;
    u8 pad1C[0x1C];
    s32 height;
};
struct SceneNpc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[6];
    u32 frame, timer;
    u8 phase, facing4, facing8, angle;
    u8 pad1C[4];
    struct Tag tag;
    u8 pad34[0x54 - 0x34];
    struct Vec target;
    u8 pad5C[9];
    u8 motionAngle;
    u8 pad66[0x23c - 0x66];
    struct Vec start;
    u8 pad244[0x2e4 - 0x244];
    u8 (*callback)(struct SceneNpc *);
    u8 pad2E8[8];
    struct Projection projection;
    u8 pad32C[0x344 - 0x32C];
    u8 groundEffect[1];
};
struct Camera {
    u8 pad[0x44];
    s32 x;
};
extern struct Camera *gSceneCamera;
void GroundEffect_EnableProjection(void *);
void Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
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
static inline void facing(struct SceneNpc *npc, u32 angle)
{
    npc->motionAngle = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}
static inline void project(struct SceneNpc *npc, struct Vec *position)
{
    struct Projection *projection = &npc->projection;
    projection->x = (position->x + gSceneCamera->x) << 12;
    projection->z = position->z << 12;
    projection->y = 0;
    projection->height = position->y << 12;
}
static inline void projectTarget(struct SceneNpc *npc)
{
    struct Projection *projection = &npc->projection;
    struct Vec *position = &npc->target;
    projection->x = (position->x + gSceneCamera->x) << 12;
    projection->z = position->z << 12;
    projection->y = 0;
    projection->height = position->y << 12;
}
void SceneNpc_UpdateProjectedMove(void *ctx, struct SceneNpc *npc, s32 tick)
{
    struct Tag *tag = &npc->tag;
    if (npc->entered) {
        s32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        GroundEffect_EnableProjection(npc->groundEffect);
        Actor_PlayAnimationEntry(npc, 3, 1, 0, 4);
        npc->phase = zero;
    }
    if (npc->callback(npc) != 1) {
        switch (npc->phase) {
            case 0:
                if (tick >= tag->duration) {
                    projectTarget(npc);
                    npc->phase++;
                } else {
                    struct PackedVec position;
                    position.x = interpolate(npc->start.x, npc->target.x, tag->duration, tick);
                    position.y = interpolate(npc->start.y, npc->target.y, tag->duration, tick);
                    position.z = interpolate(npc->start.z, npc->target.z, tag->duration, tick);
                    facing(npc, npc->angle);
                    Actor_PlayAnimationEntry(npc, 3, 1, 0, 4);
                    project(npc, (struct Vec *)&position);
                }
                break;
            case 1:
                npc->projection.x += Player_GetHorizontalVelocity(0);
                npc->done = 1;
                break;
        }
    }
}
