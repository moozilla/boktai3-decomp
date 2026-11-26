#include "scene/terrain/terrain_collision.h"
#include "scene/actor/scene_actor_behavior.h"
u8 *TerrainModifiers_FindCellOverride(u32, u32);
#include "script/script_utility.h"
static inline struct TerrainCell *CellData(u32 cell)
{
    struct TerrainCellOverride *override = (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return &override->cell;
    return &((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
}
void SceneActorBehavior_CallTerrainScript(struct RoomActor *actor)
{
    s32 values[12];
    struct ScriptUtilityArguments arguments;
    u32 script;
    {
        u32 mask = 0x80000;
        u32 *field;
        field = &actor->statusFlags;
        if (*field & mask)
            return;
    }
    {
        u32 cell = actor->terrainCell;
        u32 flags = CellData(cell)->flags;
        if (flags & 0x43)
            return;
    }
    {
        u16 *ground = &actor->groundHeight;
        if (*ground - actor->body.position.y > 256)
            return;
    }
    if (TerrainModifiers_FindCellOverride(actor->terrainCell, 1))
        return;
    script = *(u16 *)&actor->script1F4;
    if (!script)
        return;
    values[0] = actor->body.position.x;
    values[1] = actor->body.position.y;
    values[2] = actor->body.position.z;
    values[3] = actor->parameters1FC[0];
    values[4] = actor->parameters1FC[1];
    values[5] = actor->parameters1FC[2];
    values[6] = -1;
    values[7] = 0;
    values[8] = actor->category;
    values[9] = actor->paletteOffset;
    values[10] = actor->hitAttributes;
    values[11] = -1;
    arguments.count = 12;
    arguments.values = values;
    Script_Call(script, &arguments);
}
