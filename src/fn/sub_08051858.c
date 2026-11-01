#include "breakable_prop.h"
#include "scene_actor_terrain.h"
s32 BreakableProp_InitializeFromScript(struct BreakableProp *prop, u32 id, u32 unused)
{
    struct ContactPackedVector packet;
    struct ContactPosition *position;
    u32 height;
    u32 script;
    if (Script_SeekToKeyword('p')) {
        packet.x = ((Script_GetValue() >> 8) << 8) + 128;
        packet.y = Script_GetValue();
        packet.z = ((Script_GetValue() >> 8) << 8) + 128;
    } else {
        packet.x = 0;
        packet.y = 0;
        packet.z = 0;
    }
    position = (void *)&packet;
    height = SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position);
    {
        struct ContactPosition *point = (void *)&packet;
        point->y = height;
        prop->cell = SceneActorTerrain_GetCellIndex(point);
    }
    {
        struct ContactPosition *point = (void *)&packet;
        prop->height = (point->y >> 8) + 1;
    }
    TerrainModifiers_RegisterCell(prop->terrainRegistration, prop->cell, 0, prop->height, 255,
                                  0x103);
    BreakableProp_InitializeSprite(prop, &packet);
    BreakableProp_InitializeCollider(prop, id, &packet);
    script = Script_SeekToKeyword('R');
    if (script)
        prop->scriptId = Script_GetValue();
    else
        prop->scriptId = script;
    return 0;
}
