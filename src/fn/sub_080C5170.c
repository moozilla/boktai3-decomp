#include "global.h"
#include "scene_actor_reset.h"

struct ActorMotion {
    u8 pad0[0x682];
    s16 speed;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);

void SceneActor_AccelerateForTerrain(struct SceneActorRegistryResetView *actor)
{
    struct ActorMotion *motion = actor->context;
    u16 type = actor->terrainCell;
    u8 *entry = TerrainModifiers_FindCellOverride(type, 1);
    s32 maximum, acceleration;
    if (entry)
        entry += 4;
    else
        entry = (type * 4 + 12) + gTerrainMap->cells;
    if (*entry >> 4) {
        switch (actor->paletteOffset) {
            case 1:
            case 3:
            case 5:
                maximum = 320;
                acceleration = 11;
                break;
            case 0:
                maximum = 160;
                acceleration = 5;
                break;
            default:
                maximum = 256;
                acceleration = 8;
                break;
        }
    } else {
        switch (actor->paletteOffset) {
            case 1:
            case 3:
            case 5:
                maximum = 448;
                acceleration = 22;
                break;
            case 0:
                maximum = 320;
                acceleration = 10;
                break;
            default:
                maximum = 384;
                acceleration = 16;
                break;
        }
    }
    if (motion->speed < maximum)
        motion->speed += acceleration;
    else
        motion->speed = maximum;
}
