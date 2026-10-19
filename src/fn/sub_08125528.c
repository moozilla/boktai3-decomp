#include "global.h"
#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 84.7%
// Projectile step: terrain cell/height of the advanced position, collision via sub_08123194,
// random spread by heading, then notices. Structure and instruction count match; left:
// global allocation (actor r8/r10, kind sl, hits pointer assigned on both branch paths in
// the ROM = GCSE/PRE), the CellHeight inline parameter copy (ROM adds r5, r7, #0).
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}
void sub_08125528(u32 unused, struct SpreadShot *s)
{
    u16 *timer;
    struct ContactPosition *position;
    s32 angle;
    u32 cell;
    struct ContactPosition spot;
    struct ContactPosition saved;
    u8 *kind;
    u16 *hits;
    if (s->reset)
        s->reset = 0;
    timer = &s->timer;
    if (*timer) {
        (*timer)--;
        return;
    }
    {
        u16 *cellp;
        s16 *vx, *vz, *zp, *yp;
        s->cell = CellIndex(&s->position);
        position = &s->position;
        cellp = &s->cell;
        vx = &s->vx;
        vz = &s->vz;
        angle = Math_Atan2(*vx, *vz);
        saved = s->position;
        position->x += *vx;
        s->position.y = CellHeight(position, *cellp);
        zp = &s->position.z;
        *zp += *vz;
        yp = &s->position.y;
        cell = CellIndex(position);
        Collider_SetPosition(s->collider, position, 0);
        Collider_Refresh(s->collider);
        if (sub_08123194(s, angle, cell))
            s->position = saved;
        else
            *yp = CellHeight(position, cell);
        s->spritePosition = s->position;
        spot.x = position->x;
        spot.y = *yp;
        spot.z = *zp;
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
    kind = &s->kind;
    if (*kind == 2) {
        s32 i;
        for (i = 0; i <= 3; i++)
            sub_08126AD4(&spot, 1, 0, i << 6, 0x54, 1, 0);
        for (i = 0; i <= 1; i++) {
            sub_08126AD4(&spot, 1, 8, i << 6, 0x22, 1, 0);
            sub_08126AD4(&spot, 1, 14, i << 6, 0x40, 1, 0);
        }
        *timer = 8;
    } else {
        if (SpriteNotice_Create(&spot, 2, *kind) < 0)
            goto done;
        if (!SpriteNotice_CreateBetweenPositions(&saved, &spot, *kind))
            goto done;
        *timer = (Random() & 7) + 3;
    }
    if (s->hits) {
        s->hits--;
        if (*kind != 2)
            Sound_PlaySong(0x1E2);
        return;
    }
done:
    s->state = 2;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08125528);
#endif
