#include "global.h"
#include "scene_actor_chase.h"

u8 SceneActor_DispatchContextAction(void *, u32);

u8 Skeleton_RerouteToTarget(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorRegistryResetView *alt = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
    }
    if (cell == actor->terrainCell || Chase_Bits32(&gPlayers[0]->flags, 0x20000)) {
        SceneActor_DispatchContextAction(*(void **)((u8 *)actor + 0x3D0), 0);
        goto done;
    }
    {
        struct ContactPosition *targetPointer = &target;
        s32 x;
        s32 z;
        u8 *data;
        x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
        z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
        target.x = (x << 8) + 0x80;
        data = TerrainModifiers_FindCellOverride(cell, 1);
        if (data)
            data += 4;
        else
            data = gTerrainMap->cells + (cell * 4 + 12);
        {
            u32 h = (*data & 15) << 8;
            targetPointer->y = h;
        }
        targetPointer->z = (z << 8) + 0x80;
    }
    if (Chase_TryRoute(alt, *(void **)(group + 0x1FC), &actor->body.position, &target))
        return 1;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
done:
    return 0;
}
