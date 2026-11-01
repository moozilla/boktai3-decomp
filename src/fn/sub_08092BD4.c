#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_motion.h"
#include "engine.h"
#include "display_sequence_inline.h"
void CpuSet(const void *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(int, int);
struct BeePlacement {
    u32 xy, z;
};
static inline u8 *CellData(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline void Clear(u32 *fillp, u32 value, void *out)
{
    *fillp = value;
    CpuSet(fillp, out, 0x05000002);
}
void Bee_ResetRoute(struct SceneActorRegistryResetView *actor)
{
    u32 zero4;
    u8 *bytes;
    u8 *context = actor->context;
    struct ContactPosition *source;
    u32 zero;
    s32 height, slope;
    u8 *c;
    u32 cellIndex;
    u8 zero8;
    u32 fill;
    u8 *first;
    first = context + 0x3DD;
    zero = 0;
    zero8 = 0;
    *first = zero;
    *(u16 *)(context + 0x430) = zero;
    context[0x432] = zero8;
    context[0x433] = 1;
    *(u16 *)(context + 0x434) = zero;
    context[0x436] = zero8;
    context[0x437] = zero8;
    *(u32 *)(context + 0x3D4) = zero;
    source = (struct ContactPosition *)(context + 0x488);
    actor->body.position = *source;
    {
        s32 x = source->x >> 8, z = source->z >> 8, index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        actor->terrainCell = index;
        cellIndex = actor->terrainCell;
    }
    {
        bytes = context + 0x488;
        c = CellData(cellIndex);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= bytes[4];
                break;
            case 2:
                height -= bytes[0];
                break;
        }
    }
    actor->groundHeight2BE = height;
    actor->field2EA = actor->body.position.y - height;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind) {
        *(struct BeePlacement *)((u8 *)actor->drawable + 0x1C) =
            *(struct BeePlacement *)&actor->body.position;
    } else {
        *(struct BeePlacement *)((u8 *)actor->drawable + 0x40) =
            *(struct BeePlacement *)&actor->body.position;
    }
    {
        u8 *dst = context + 0x470;
        zero4 = 0;
        Clear(&fill, zero4, dst);
    }
    {
        u8 *dst = context + 0x478;
        Clear(&fill, zero4, dst);
    }
    {
        u8 *dst = context + 0x480;
        Clear(&fill, zero4, dst);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    DisplaySequence_Step(actor->drawable);
}
