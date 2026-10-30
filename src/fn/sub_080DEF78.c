#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 63.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
static inline s32 Direction(s32 x)
{
    return x >= 0 ? x >> 6 : -(-x >> 6);
}
u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, s32);
// Branch layout, pointer lifetimes and register allocation still differ.
// confidence=low: searches along the facing direction and starts a reachable route.
#define Dog_TryFacingRoute sub_080DEF78
void Dog_TryFacingRoute(struct SceneActorRegistryResetView *a, u32 saved)
{
    struct ContactPosition target;
    u8 *context = a->context;
    u8 *group = (u8 *)(&a->group);
    s32 tries = 0;
    s32 selected;
    u32 k1;
    do {
        s32 cell = a->terrainCell;
        s32 height = Chase_HeightAt(cell, &a->body.position);
        u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
        s32 kind;
        s32 next;
        s32 count;
        if (data)
            data += 4;
        else
            data = (gTerrainMap->cells + (cell * 4)) + 12;
        kind = (*data) >> 4;
        switch (Direction(a->body.direction + 32) & 3) {
            case 0:
                next = cell + 1;
                count = 0;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, kind))
                        break;
                    next++;
                } while ((++count) < 1);
                selected = next - 1;
                break;

            case 1:
                next = cell + (*((s16 *)(gTerrainMap->cells + 4)));
                count = 0;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, kind))
                        break;
                    next += *((s16 *)(gTerrainMap->cells + 4));
                } while ((++count) < 1);
                selected = next - (*((s16 *)(gTerrainMap->cells + 4)));
                break;

            case 2:
                next = cell - 1;
                count = 0;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, kind))
                        break;
                    next--;
                } while ((++count) < 1);
                selected = next + 1;
                break;

            case 3:
                next = cell - (*((s16 *)(gTerrainMap->cells + 4)));
                count = 0;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, kind))
                        break;
                    next -= *((s16 *)(gTerrainMap->cells + 4));
                } while ((++count) < 1);
                selected = next + (*((s16 *)(gTerrainMap->cells + 4)));
                break;

            default:
                selected = a->terrainCell;
        }

        if (*((s8 *)(context + (k1 = 0x6A6))))
            a->body.direction += 64;
        else
            a->body.direction -= 64;
        if (selected != a->terrainCell)
            break;
    } while ((++tries) <= 3);
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 k2 = 127;
        if (((u8)gRandomTable[gRandomIndex]) <= k2)
            context[k1] = 1;
        else
            context[k1] = 0;
    }
    a->body.direction = saved;
    {
        s32 x = Mod(selected, *((s16 *)(gTerrainMap->cells + 4)));
        s32 z = Div(selected, *((s16 *)(gTerrainMap->cells + 4)));
        u8 *data;
        target.x = (x << 8) + 128;
        data = TerrainModifiers_FindCellOverride(selected, 1);
        if (data)
            data += 4;
        else
            data = (gTerrainMap->cells + (selected * 4)) + 12;
        target.y = ((*data) & 15) << 8;
        target.z = (z << 8) + 128;
    }
    if (Chase_TryRoute(a, *((void **)(group + 0x1FC)), &a->body.position, &target)) {
        context[0x6A7]++;
        return;
    }
    Callback_InvokeActor(a, *((void **)(group + 0x224)));
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080DEF78);
#endif
