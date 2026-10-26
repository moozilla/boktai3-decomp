#include "scene_actor_placement.h"

#ifdef NONMATCHING
// fuzzy: 45.4%
// scope shape 116 (differences once low registers are set aside)
// confidence=low: places an actor on the terrain cell under its path point, sets its height and resets its control state
void sub_08077444(struct SceneActorPlacementView *actor, u32 pathId, u32 pathArg, u32 direction)
{
    struct EngineMap *map;
    u8 *override;
    u8 *cellData;
    u16 *halfword13A;
    s32 tileX, tileZ;
    u32 cell;
    u8 flagsByte;
    s32 height;
    s32 type;
    const void *ctl;
    struct ContactPosition *pos;

    pos = &actor->position;
    if (actor->flags2CC & 1) {
        actor->flags2CC &= ~4;
        actor->route.fieldC = actor->route.parameterC;
        Path_GetPointPositionById((u16 *)&actor->position, pathId, pathArg);
        tileX = pos->x >> 8;
        tileZ = pos->z >> 8;
        if (tileX < 0 || tileZ < 0 || tileX >= gTerrainWidth || tileZ >= gTerrainHeight)
            cell = 0;
        else
            cell = gTerrainMap->rows[tileZ] + tileX;
        actor->terrainCell = cell;
        cell = actor->terrainCell;
        override = TerrainModifiers_FindCellOverride(cell, 1);
        if (override)
            cellData = override + 4;
        else
            cellData = gTerrainMap->cells + 12 + cell * 4;
        flagsByte = *cellData;
        type = flagsByte >> 4;
        height = (flagsByte & 0xF) << 8;
        if (type == 1)
            height -= (u8)pos->z;
        else if (type == 2)
            height -= (u8)pos->x;
        pos->y = height;
        if (actor->drawableKind == 0)
            ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position =
                actor->position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->position;
        actor->direction = direction;
        if (actor->flags2CC & 0x200000) {
            actor->flags2D0 &= ~0x01000000;
            actor->flags2D4 &= ~0x200;
            actor->group.control.mode = 0;
            actor->group.control.counter = 0;
            actor->group.control.timer = 0;
        }
        halfword13A = (u16 *)(actor->unknown118 + 0x22);
        *halfword13A &= ~4;
        if (actor->drawableKind == 0)
            ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags |= 2;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags |= 2;
        if (actor->drawableKind == 0)
            ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags &= ~1;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags &= ~1;
        actor->state2A8 = 3;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        ctl = actor->resetControl220;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = 0xC;
        actor->control = ctl;
        actor->field2C8 = 0;
        actor->flags114 &= ~1;
        actor->field104 = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08077444);
#endif
