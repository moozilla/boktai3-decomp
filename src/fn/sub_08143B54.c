#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 94.8%
// fix.py best version
// fix.py best version (reprinted C)
// confidence=low: limits the tether target distance and clips its position against terrain.

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

struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
u32 Player_TestTetherTarget(void *, void *);
void TetherActor_SetInteractionTarget(void *, u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline s32 indexAt(struct Position *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 heightAt(struct Position *position)
{
    u8 *p = properties(position);
    s32 surface = *p >> 4, height = (*p & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
// MATCHING_CONCESSION: register-pinning
void Player_ConstrainTetherTarget(struct Actor *input, s32 dx, s32 dz, s32 radius)
{
    register s32 oz asm("r8") = scale(70 * dz);
    register struct Actor *actor asm("r6") = input;
    register s32 ox asm("r7") = scale(70 * dx);
    struct Position *target = TetherActor_GetPosition();
    s32 x = (target->x - actor->position.x) - ox;
    s32 z = (target->z - actor->position.z) - oz;
    if (((x * x) + (z * z)) > (radius * radius)) {
        s32 angle = Math_Atan2(x, z);
        struct PackedVec candidate;
        candidate.x = (actor->position.x + scale(gUnk_086149C4[(angle + 64) & 255] * radius)) + ox;
        candidate.z = (actor->position.z + scale(gUnk_086149C4[angle & 255] * radius)) + oz;
        candidate.x = ((target->x * 3) + candidate.x) >> 2;
        candidate.z = ((target->z * 3) + candidate.z) >> 2;
        if (!Player_TestTetherTarget(actor, &candidate)) {
            u16 ax = actor->position.x;
            s16 cellx = ((s16)ax) >> 8;
            s32 cellz = actor->position.z >> 8;
            s32 current;
            s32 next;
            if ((((cellx < 0) || (cellz < 0)) || (((u32)cellx) >= gTerrainWidth)) ||
                (((u32)cellz) >= gTerrainHeight))
                current = 0;
            else
                current = gTerrainMap->rows[cellz] + cellx;
            asm("" : : : "memory");
            next = indexAt((void *)(&candidate));
            if (current == (next + 1))
                candidate.x = ((((s16)ax) >> 8) << 8) + 2;
            else if (current == (next - 1))
                candidate.x = (((((s16)ax) >> 8) + 1) << 8) - 2;
            else {
                s32 width = *((s16 *)(gTerrainMap->cells + 4));
                if (current == (next + width))
                    candidate.z = ((actor->position.z >> 8) << 8) + 2;
                else if (current == (next - width))
                    candidate.z = (((actor->position.z >> 8) + 1) << 8) - 2;
                else {
                    candidate.x = ax;
                    candidate.z = actor->position.z;
                }
            }
            if (!TetherActor_GetStatusFlags())
                candidate.y = heightAt((void *)(&candidate));
        } else if (!TetherActor_GetStatusFlags())
            candidate.y = heightAt((void *)(&candidate));
        x = actor->position.x - candidate.x;
        z = actor->position.z - candidate.z;
        angle = (u16)Math_Atan2(x, z);
        TetherActor_SetInteractionTarget(&candidate, ((((u16)angle) + 0x70) >> 5) & 7);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_ConstrainTetherTarget);
#endif
