#include "global.h"
#include "engine.h"

// confidence=low: validates terrain for a tether target and snaps to the tether actor on flagged cells.

struct Position {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[0x30];
    struct Position position;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 TetherActor_GetStatusFlags(void);
u32 Player_IsStatusBit4ActionActive(u32);
struct Position *TetherActor_GetPosition(void);
void Sound_PlaySong(u32);
void ActorSequence_Begin(void *, s32, s32);
static inline u8 *properties(struct Position *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    s32 cell;
    u8 *p;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else {
        struct Map *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    return p;
}
static inline s32 absolute(s32 v)
{
    if (v < 0)
        v = -v;
    return v;
}
u32 Player_TestTetherTarget(struct Actor *actor, struct Position *candidate)
{
    u8 *p = properties(candidate);
    u32 flags = *(u16 *)(p + 2);
    s32 height, surface;
    struct Position *target;
    if (flags & 1)
        return 0;
    if (!TetherActor_GetStatusFlags()) {
        if (Player_IsStatusBit4ActionActive(flags))
            return 0;
        if (flags & 0x40)
            return 0;
    }
    p = properties(candidate);
    surface = *p >> 4;
    height = (*p & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)candidate->z;
            break;
        case 2:
            height -= (u8)candidate->x;
            break;
    }
    if (absolute(height - actor->position.y) > 255)
        return 0;
    if (flags & 0x800) {
        target = TetherActor_GetPosition();
        if (absolute(target->x - actor->position.x) > 255 ||
            absolute(target->z - actor->position.z) > 255) {
            Sound_PlaySong(0xDA);
            ActorSequence_Begin(actor, 0, 0);
        }
        *candidate = *target;
    }
    return 1;
}
