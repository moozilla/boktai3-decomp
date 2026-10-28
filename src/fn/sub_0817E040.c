#include "global.h"
#include "ram.h"

struct VolumeActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[2];
    s16 position[4];
    s16 velocity[4];
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 direction4, direction8;
    u8 state, padAB, action, subaction, choice, pending, queued, restart, finished, special;
    u8 padB4[0x10];
    u32 actionTimer;
    u8 padC8[4];
    u8 requestedDirection;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[4];
    u8 *target;
    u8 pad110[4];
    s8 alternateDirection;
    u8 pad115[0x173];
    u8 bounds[0x1C];
    s16 destination[4];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x5F];
    u8 effect[0x13C];
    s16 height;
    u8 pad46A[6];
    u8 slowDown, impactDirection, speed, impactCount;
    u8 pad474[0x6C];
    u8 movementMode;
    u8 pad4E1;
    u8 reached, pathId, pointId;
    u8 pad4E5[3];
    u8 transition, previousChoice, decisionMode, decisionIndex;
    u8 pad4EC[8];
    s16 attackFlags;
    u8 pad4F6[6];
    s16 parameter;
    u8 pad4FE[0x722];
    u8 pathPosition[8];
};
extern const s16 gUnk_086149C4[];
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void ProximityVolume_TranslateBounds(void *, void *);
void Sound_PlaySong(u32);
s32 ActorPosition_TestPath(void *, void *);
s32 SceneVolume_TestPlayerPosition(void *);
s32 SpriteComponent_GetCollisionStatus(void *);
void SceneParticleBurst_CreateWithPause(void *, void *, void *, int, int, int);
int Math_Atan2(int, int);
void ActorEffect_SpawnAtAngle(void *, void *, int);
u8 Callback_InvokeActor(void *, void *);
static inline void SetDirection(struct VolumeActor *p, int angle)
{
    p->direction = angle;
    {
        s32 d = (p->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        p->direction4 = mask;
    }
    {
        s32 d = (p->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        p->direction8 = mask;
    }
}
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

struct TerrainMap {
    u32 unknown;
    u8 *cells;
    u8 pad8[0x1C];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(u32, int);
static inline int TerrainHeight(s16 *position)
{
    s32 x = position[0] >> 8, z = position[2] >> 8;
    u32 cell;
    u8 *data;
    u32 shape, flags, height;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = ((struct TerrainMap *)gTerrainMap)->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct TerrainMap *map = ((struct TerrainMap *)gTerrainMap);
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    {
        u32 mask = 15;
        mask &= flags;
        height = mask << 8;
    }
    switch (shape) {
        case 1:
            height -= *(u8 *)&position[2];
            break;
        case 2:
            height -= *(u8 *)&position[0];
            break;
    }
    return height;
}
static inline void SetVelocity(s16 *velocity, u32 angle, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    velocity[0] = x;
    velocity[1] = zero;
    velocity[2] = Scale(gUnk_086149C4[angle] * speed);
}
// confidence=low: decays impact velocity and height before landing animations
void SceneVolume_UpdateKnockbackLanding(struct VolumeActor *p, s32 elapsed)
{
    struct VolumeActor *q = p;
    if (p->restart) {
        p->restart = 0;
        SetDirection(p, p->impactDirection + 128);
        SceneActors_StartDirectionalAnimation(p, &p->animation, p->animationState, 0x114, 1, 1, 4);
        if (p->attackFlags & 1)
            Sound_PlaySong(0x542);
        else
            Sound_PlaySong(0x53D);
        q->attackFlags &= ~1;
        p->phase = 0;
        p->timer = 0;
        q->height = 300;
    }
    {
        u32 mask = 0x400004;
        p->flags |= mask;
    }
    switch (p->phase) {
        case 0:
            if (elapsed > 3) {
                SceneActors_StartDirectionalAnimation(p, &q->animation, q->animationState, 0x11A, 1,
                                                      1, 4);
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            SetVelocity(p->velocity, q->impactDirection, q->speed);
            p->position[1] = q->height + TerrainHeight(p->position);
            q->speed = (q->speed * 3) >> 2;
            if (elapsed > 3) {
                if (q->height >= elapsed * 8)
                    q->height -= elapsed * 8;
                else
                    q->height = 0;
            }
            if (q->speed <= 19 && q->height == 0) {
                Sound_PlaySong(0x2A8);
                p->phase = 2;
                p->timer = 0;
                SceneActors_StartDirectionalAnimation(p, &q->animation, q->animationState, 0x11C, 2,
                                                      1, 4);
            }
            break;
        case 2:
            if (++p->timer > 19) {
                u32 phase = 3;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;
        case 3:
            if (p->timer == 0) {
                SceneActors_StartDirectionalAnimation(p, &q->animation, q->animationState, 0x11E, 2,
                                                      1, 4);
                p->timer++;
            }
            if (p->finished) {
                p->pending = 1;
                p->phase = 4;
                p->timer = 0;
            }
            break;
        case 4:
            return;
    }
    {
        u32 mask = 1;
        p->flags |= mask;
    }
}
