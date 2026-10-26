#include "scene_actor_behavior.h"
u8 *TerrainModifiers_FindCellOverride(u32, u32);
#include "script_utility.h"
static inline u8 *CellData(u32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *actor)
{
    s32 values[12];
    struct ScriptUtilityArguments arguments;
    u32 script;
    {
        u32 mask = 0x80000;
        u32 offset = 0x2D0;
        u32 *field;
        field = (void *)((u32)actor + offset);
        if (*field & mask)
            return;
    }
    {
        u32 offset = 0x2F4;
        u32 cell = *(u16 *)((u32)actor + offset);
        u32 flags = *(u16 *)(CellData(cell) + 2);
        u32 mask = 0x43;
        if (flags & mask)
            return;
    }
    {
        u32 offset = 0x2BE;
        u16 *ground = (void *)((u32)actor + offset);
        if (*ground - actor->position.y > 256)
            return;
    }
    if (TerrainModifiers_FindCellOverride(actor->terrainCell, 1))
        return;
    script = *(u16 *)&actor->script1F4;
    if (!script)
        return;
    values[0] = actor->position.x;
    values[1] = actor->position.y;
    values[2] = actor->position.z;
    values[3] = actor->parameters1FC[0];
    values[4] = actor->parameters1FC[1];
    values[5] = actor->parameters1FC[2];
    values[6] = -1;
    values[7] = 0;
    values[8] = actor->category;
    values[9] = actor->paletteOffset;
    values[10] = actor->parameter2D8;
    values[11] = -1;
    arguments.count = 12;
    arguments.values = values;
    Script_Call(script, &arguments);
}
