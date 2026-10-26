#include "global.h"
#include "engine.h"
struct HresvelgrPosition {
    s16 x, y, z, pad;
};
struct HresvelgrMotionFlags {
    u8 unknown0[6];
    u16 flags;
};
struct HresvelgrSprite {
    u32 flags;
    u8 unknown4[0x18];
    struct HresvelgrPosition position;
    u8 unknown24[0x24];
};
struct HresvelgrMotion {
    u8 unknown0[0x18];
    struct HresvelgrSprite sprite;
    struct HresvelgrMotionFlags motion;
    u8 unknown68[0x4c];
    struct HresvelgrPosition origin;
    struct HresvelgrPosition velocity;
    struct HresvelgrPosition offset;
    u8 unknownCC[0x6c];
    u16 state, stateFrame;
};
static inline void add(struct HresvelgrPosition *d, struct HresvelgrPosition *a,
                       struct HresvelgrPosition *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
static inline void setFlags(struct HresvelgrMotionFlags *p, u32 mask)
{
    p->flags = mask | p->flags;
}
static inline void setState(struct HresvelgrMotion *p, u32 state)
{
    p->state = state;
    p->stateFrame = 0;
}
struct HresvelgrTerrain {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1c];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 cellIndex(struct HresvelgrPosition *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct HresvelgrPosition *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline void clear(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
static inline void advance(struct HresvelgrPosition *d, struct HresvelgrPosition *v)
{
    d->x += v->x;
    d->y += v->y;
    d->z += v->z;
}
void Hresvelgr_UpdateFallingMotion(struct HresvelgrMotion *p)
{
    struct HresvelgrSprite *sprite = &p->sprite;
    s32 height;
    if (!p->stateFrame) {
        setFlags(&p->motion, 4);
        p->sprite.flags &= ~1;
        clear(&p->offset);
        ++p->stateFrame;
    }
    advance(&p->sprite.position, &p->velocity);
    --p->velocity.y;
    height = groundHeight(&p->sprite.position);
    if (sprite->position.y <= height) {
        sprite->position.y = height;
        setState(p, 2);
    }
}
