#include "global.h"
#include "scene_actor_reset.h"
/* Spread-shot projectile step: advance, collide, spawn impact notices */
// Projectile step: terrain cell/height of the advanced position, collision via sub_08123194,
// random spread by heading, then notices. Fields are written as s->field throughout: the ROM's
// late `adds rX, rT, #0` / `mov sb, rT` copies are GCSE/PRE reaching registers, not locals.
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
u8 sub_08123194(void *, s32, u32);
void sub_08126AD4(struct ContactPosition *, u32, u32, u32, u32, u32, u32);
s32 SpriteNotice_Create(struct ContactPosition *, u32, u32);
u8 SpriteNotice_CreateBetweenPositions(struct ContactPosition *, struct ContactPosition *, u32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
struct SpreadShot {
    u8 unknown0[0xC];
    struct ContactPosition spritePosition;
    u8 unknown14[0x60];
    struct ContactPosition position;
    s16 vx;
    u8 unknown7E[2];
    s16 vz;
    u8 unknown82[2];
    u8 collider[0x54];
    u16 hits;
    u8 kind;
    u8 reset;
    u16 cell;
    u8 unknownDE;
    u8 state;
    u16 timer;
};
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline u32 Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
void SpreadShot_Step(u32 unused, struct SpreadShot *s)
{
    u16 *timer;
    s32 angle;
    u32 cell;
    struct ContactPosition spot;
    struct ContactPosition saved;
    if (s->reset)
        s->reset = 0;
    {
        u16 t = s->timer;
        timer = &s->timer;
        if (t) {
            *timer = t - 1;
            return;
        }
    }

    {
        u16 *cellp;
        s16 *vx;
        s->cell = CellIndex(&s->position);
        cellp = &s->cell;
        vx = &s->vx;
        angle = Math_Atan2(*vx, s->vz);
        saved = s->position;
        s->position.x += *vx;
        s->position.y = CellHeight(&s->position, *cellp);
        s->position.z += s->vz;
        cell = CellIndex(&s->position);
        Collider_SetPosition(s->collider, &s->position, 0);
        Collider_Refresh(s->collider);
        if (sub_08123194(s, angle, cell))
            s->position = saved;
        else
            s->position.y = CellHeight(&s->position, cell);
        s->spritePosition = s->position;
        spot.x = s->position.x;
        spot.y = s->position.y;
        spot.z = s->position.z;
    }
    switch (angle >> 6) {
        case 0:
        case 2:
            spot.z += Mod(Random(), 120) - 60;
            break;
        case 1:
        case 3:
            spot.x += Mod(Random(), 120) - 60;
            break;
    }
    if (s->kind == 2) {
        s32 i;
        for (i = 0; i <= 3; i++)
            sub_08126AD4(&spot, 1, 0, i << 6, 0x54, 1, 0);
        for (i = 0; i <= 1; i++) {
            sub_08126AD4(&spot, 1, 8, i << 6, 0x22, 1, 0);
            sub_08126AD4(&spot, 1, 14, i << 6, 0x40, 1, 0);
        }
        *timer = 8;
    } else {
        if (SpriteNotice_Create(&spot, 2, s->kind) < 0)
            goto done;
        if (!SpriteNotice_CreateBetweenPositions(&saved, &spot, s->kind))
            goto done;
        *timer = (Random() & 7) + 3;
    }
    if (s->hits) {
        s->hits--;
        if (s->kind != 2)
            Sound_PlaySong(0x1E2);
        return;
    }
done:
    s->state = 2;
}
