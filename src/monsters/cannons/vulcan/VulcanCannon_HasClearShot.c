#include "monsters/cannons/vulcan_cannon_worker.h"
#include "objects/hazards/animated_hazard.h"
#include "recovered/contact_sample_vector_layout.h"
s32 Div(s32, s32);
static inline void subtract(struct SceneVec *v, struct SceneVec *a, struct SceneVec *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline s32 cellIndex(struct SceneVec *v)
{
    s32 x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cellData(s32 index)
{
    u8 *override = TerrainModifiers_FindCellOverride(index, 1);
    if (override)
        return override + 4;
    return index * 4 + 12 + gTerrainMap->cells;
}
s32 VulcanCannon_HasClearShot(struct VulcanCannon *p)
{
    struct SceneVec delta;
    struct ContactSampleVector sample;
    s32 count = Div(p->range, 256);
    s32 i;
    subtract(&delta, Player_GetEffectivePosition(0), (struct SceneVec *)&p->display.position);
    for (i = 1; i < count; i++) {
        s32 index;
        u8 *cell;
        sample.x = p->display.position.x + Div(delta.x * i, count);
        sample.y = p->display.position.y + 128;
        sample.z = p->display.position.z + Div(delta.z * i, count);
        index = cellIndex((struct SceneVec *)&sample);
        cell = cellData(index);
        if (((cell[0] & 15u) << 8) > p->display.position.y + 128)
            return 0;
    }
    return 1;
}
