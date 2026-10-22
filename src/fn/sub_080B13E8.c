#include "scene_actor_terrain.h"
struct VornTerrainChoice {
    u8 unknown0[0x6B0];
    u16 turnX, unknown6B2, turnZ;
    u8 unknown6B6[3];
    u8 axis;
};
struct VornProbe {
    s32 x : 16;
    s32 y : 16;
    s16 z, pad;
};
static inline s32 Height(struct ContactPosition *p)
{
    s32 cell = SceneActorTerrain_GetCellIndex(p);
    u32 kind;
    s32 height;
    u8 *data = SceneActorTerrain_GetCell(cell);
    kind = *data >> 4;
    height = (*data & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= ((u8 *)p)[4];
    goto done;
slopeX:
    height -= ((u8 *)p)[0];
done:
    return height;
}
// confidence=low: biases turn weights toward a neighboring cell with higher terrain.
s32 Vorn_SelectUphillTurn(struct VornTerrainChoice *context, struct ContactPosition *position)
{
    struct VornProbe probe;
    s32 height, heightX, heightZ;
    s32 result;
    height = Height(position);
    probe = *(struct VornProbe *)position;
    probe.x += 256;
    heightX = Height((void *)&probe);
    probe = *(struct VornProbe *)position;
    probe.z += 256;
    heightZ = Height((void *)&probe);
    if (heightX - height > 0) {
        context->turnX += 110;
        context->turnZ += 100;
        context->axis = 0;
        result = 1;
    } else if (heightZ - height <= 0) {
        result = 0;
    } else {
        context->turnX += 100;
        context->turnZ += 110;
        context->axis = 1;
        result = 1;
    }
    return result;
}
