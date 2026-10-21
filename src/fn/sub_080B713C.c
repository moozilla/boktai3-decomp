// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_terrain.h"

s32 SceneActorTerrain_FindOpenNeighborDirection(struct SceneActorTerrainView *actor, s32 angle)
{
    s32 direction = ((angle + 0x10) & 0xff) >> 6;
    s32 opposite = (direction + 2) & 3;
    u8 mask;
    s32 i;
    i = 0;
    mask = 3;
    asm("" : "+r"(mask));

    for (; i <= 3; i++) {
        if (direction != opposite) {
            if (SceneActorTerrain_GetNeighborCell(actor, direction) >= 0)
                return direction;
        }
        direction = (direction + 1) & mask;
    }
    return -1;
}
