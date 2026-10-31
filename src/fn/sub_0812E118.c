#include "global.h"
#include "engine.h"
#include "contact_actor.h"
#include "scene_actor_components.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);

struct TerrainCell {
    u8 heightAndShape;
    u8 unk_01;
    u16 flags;
};

struct TerrainGrid {
    u32 unk_00;
    s16 width, height;
    s16 originX, originZ;
    struct TerrainCell cells[1];
};

static inline s32 MapWidth(void)
{
    return ((struct TerrainGrid *)gTerrainMap->cells)->width;
}

static inline struct TerrainCell *CellProperties(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        return (struct TerrainCell *)(properties + 4);
    return &((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
}

static inline s32 CellHeight(s32 cell, const struct ContactPosition *position)
{
    struct TerrainCell *properties = CellProperties(cell);
    u32 shape = properties->heightAndShape >> 4;
    s32 height = (properties->heightAndShape & 15) << 8;
    switch (shape) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline void CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, MapWidth());
    s32 z = Div(cell, MapWidth());
    position->x = ((u32)x << 8) + 128;
    position->y = (CellProperties(cell)->heightAndShape & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}

static inline s32 Absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}

// Tentative: test a route cell against the starting cell's slope and height.
u8 SceneActorTerrain_CanTraverseCell(s32 startCell, s32 cell, s32 startHeight, u32 startShape)
{
    struct ContactPosition center;
    u32 shape;
    s32 height;
    s32 allowHeightStep;

    /* The original query samples slopes before the position is filled. */
    shape = CellProperties(cell)->heightAndShape >> 4;
    height = CellHeight(cell, &center);
    CellCenter(cell, &center);
    allowHeightStep = 0;
    if ((shape == 2 && startShape == 0) || (startShape == 2 && shape == 0)) {
        if (Absolute(startCell - cell) == 1)
            allowHeightStep = 1;
    } else if ((shape == 1 && startShape == 0) || (startShape == 1 && shape == 0)) {
        if (Absolute(startCell - cell) == MapWidth())
            allowHeightStep = 1;
    } else if ((shape == 1 && startShape == 1) || (shape == 2 && startShape == 2))
        allowHeightStep = 1;

    {
        u32 flags = CellProperties(cell)->flags;
        if (flags & 0x43)
            return 0;
    }
    if (allowHeightStep) {
        s32 difference;
        height >>= 8;
        startHeight >>= 8;
        difference = Absolute(height - startHeight);
        if (difference > 1)
            return 0;
    } else if (height != startHeight)
        return 0;
    return 1;
}

static inline s32 CellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

/* Test the floor ahead along the first moving horizontal axis. */
u8 SceneActorTerrain_IsBelowForwardFloor(struct SceneActorBody *body)
{
    struct ContactPackedVector offset;
    struct ContactPackedVector point;
    s32 cell;
    s32 height;
    if (body->motion.x != 0) {
        if (body->motion.x < 0) {
            offset.x = -32;
            offset.z = 0;
        } else {
            offset.x = 32;
            offset.z = 0;
        }
    } else if (body->motion.z != 0) {
        if (body->motion.z < 0) {
            offset.x = 0;
            offset.z = -32;
        } else {
            offset.x = 0;
            offset.z = 32;
        }
    } else
        return 0;
    point.x = body->position.x + body->motion.x + offset.x;
    point.y = body->position.y + body->motion.y;
    point.z = body->position.z + body->motion.z + offset.z;
    cell = CellAt((struct ContactPosition *)&point);
    height = (CellProperties(cell)->heightAndShape & 15) << 8;
    if (body->position.y < height)
        return 1;
    return 0;
}
