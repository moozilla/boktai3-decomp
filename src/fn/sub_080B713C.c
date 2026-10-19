#include "global.h"
#include "scene_actor_terrain.h"

#ifdef NONMATCHING
// fuzzy: 91.2%
// foundry draft (worker best 90.9%); what differs is mostly register allocation and instruction order
#define SceneActorTerrain_FindOpenNeighborDirection sub_080B713C

s32 sub_080B713C(struct SceneActorTerrainView *actor, s32 angle)
{
    s32 direction = ((angle + 0x10) & 0xff) >> 6;
    u8 mask = 3;
    s32 opposite = (direction + 2) & 3;
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        if (direction != opposite)
        {
            if (SceneActorTerrain_GetNeighborCell(actor, direction) >= 0)
                return direction;
        }
        direction = (direction + 1) & mask;
    }
    return -1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B713C);
#endif
