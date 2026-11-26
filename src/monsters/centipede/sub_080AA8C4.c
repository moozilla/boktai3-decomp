#include "monsters/centipede/tentacle_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_terrain.h"
#include "monsters/ghoul_disc.h"
static inline u32 WordFlag(struct ChaseActor *p, u32 mask)
{
    return mask & p->base.statusFlags;
}
static inline s32 Height(s32 cell, struct ContactPosition *position)
{
    struct ContactPosition *p = position;
    u8 attr = *SceneActorTerrain_GetCell(cell);
    u8 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= (u8)p->z;
            break;
        case 2:
            height -= (u8)p->x;
            break;
    }
    return height;
}
void TentacleActor_AlignSegmentsToTerrain(struct ChaseActor *actor)
{
    struct TentacleBody *trail;
    s32 i;
    u32 positionY = (u32) & ((struct TentacleBody *)0)->segments[0].position.y;
    if (SceneActor_ActionsBlocked(actor))
        return;
    trail = actor->base.context;
    if (WordFlag(actor, 0x100000))
        return;
    for (i = 0; i < trail->segmentCount; i++) {
        struct TentacleAnimationSegment *segment = &trail->segments[i];
        struct ContactPosition *position = &segment->position;
        if (TentacleSpread_Test(segment, 2) && (!TentacleSpread_Test(segment, 1))) {
            s32 rawHeight = Height(SceneActorTerrain_GetCellIndex(position), position);
            s32 height = rawHeight;
            s16 y;
            y = segment->position.y;
            if (y != height) {
                if (y < height) {
                    s32 delta = height - y;
                    if (Ghoul_Abs(delta) > 255)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TentacleAnimationSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else if (Ghoul_Abs(delta) > 255)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TentacleAnimationSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TentacleAnimationSegment))) +
                                  positionY)) = height;
                } else {
                    s32 delta;
                    height = rawHeight - y;
                    delta = height;
                    if (Ghoul_Abs(delta) > 256)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TentacleAnimationSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TentacleAnimationSegment))) +
                                  positionY)) += Div(delta, 10);
                }
            }
        }
    }
}
