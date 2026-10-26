#include "global.h"
#include "scene_grid.h"
#include "area_effect_link.h"
/* Bounce-shot terrain probe: may the shot enter the next cell, else slide it along a wall or up
 * a slope toward the player. The match depends on GCSE's expression hash table size (insns at
 * GCSE / 2) and on the pseudo numbers of `cell`, `start` and `preferred`: they order four spilled
 * PRE registers' stack slots. SlopeSurface's u16/s16 round trip and the function-scope `cell`
 * set both; see notes/cloud-08122DF4.md before restructuring. */
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
void Memory_Clear(void *, s32);
struct BouncePosition {
    s16 x, y, z, pad;
};
struct BounceShot {
    u8 unknown0[0x74];
    struct BouncePosition position;
    s16 vx;
    u8 unknown7E[2];
    s16 vz;
    u8 unknown82[0x58];
    u8 kind;
    u8 unknownDB;
    u16 cell;
    u8 unknownDE;
    u8 state;
};
static inline u8 *Properties(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
/* Movement-blocking attribute bits of a cell (the halfword after its height byte). */
static inline u16 CellAttributes(s32 index)
{
    return *(u16 *)(Properties(index) + 2);
}
/* Height of a cell under a position; slopes (shape 1, 2) subtract the low byte of z or x. */
static inline u32 CellHeight(u8 *properties, struct BouncePosition *position)
{
    u32 shape = (*properties >> 4) & 15;
    u32 height = (*properties & 15) << 8;
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
/* Surface height a shot riding a slope cell takes on (its y). */
static inline u16 SlopeSurface(u8 *properties, struct BouncePosition *position)
{
    s16 height = CellHeight(properties, position);
    return height;
}
/* Can the shot enter `next` from `current` moving in quadrant `direction`? */
static inline u8 CanEnter(struct BouncePosition *position, u32 current, u32 next, s32 angle)
{
    switch (((angle + 0x20) >> 6) & 3) {
        case 0:
            if (*Properties(current) >> 4 == 2)
                return 1;
            if (*Properties(next) >> 4 == 2)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 1:
            if (*Properties(current) >> 4 == 1)
                return 1;
            if (*Properties(next) >> 4 == 1)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 2:
            if (*Properties(current) >> 4 == 2)
                return 1;
            if (*Properties(next) >> 4 == 2)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 3:
            if (*Properties(current) >> 4 == 1)
                return 1;
            if (*Properties(next) >> 4 == 1)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
    }
    return 0;
}
/* Walk from `cell` towards `target` along the row, then the column, over cells of the same
 * height; a slope cell ends the walk (the shot rides it). Returns 1 if the walk reaches. */
static inline u8 Seek(struct BouncePosition *position, s32 cell, s32 target)
{
    s32 level = position->y >> 8;
    s32 width = SceneGrid_Width();
    s32 total = width * *(s16 *)(gTerrainMap->cells + 6);
    s32 i, column;
    if (cell < target) {
        i = cell + 1;
        column = Mod(i, width);
        while (column < SceneGrid_Width()) {
            if ((*Properties(i) & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (*Properties(i) >> 4 == 2) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
            i++;
            column++;
        }
        for (i = cell + SceneGrid_Width(); total > i; i += SceneGrid_Width()) {
            if ((*Properties(i) & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (*Properties(i) >> 4 == 1) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
        }
        return 0;
    } else {
        i = cell - 1;
        column = Mod(i, width);
        while (column >= 0) {
            if ((*Properties(i) & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (*Properties(i) >> 4 == 2) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
            i--;
            column--;
        }
        for (i = cell - SceneGrid_Width(); i >= 0; i -= SceneGrid_Width()) {
            if ((*Properties(i) & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (*Properties(i) >> 4 == 1) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
        }
    }
    return 0;
}
static inline void Delta(struct BouncePosition *out, struct BouncePosition *a,
                         struct BouncePosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
u8 sub_08123194(struct BounceShot *s, s32 angle, u32 next)
{
    struct BouncePosition *position;
    u16 *cellp;
    s32 quadrant;
    s32 result;
    s32 speed;
    s32 cell;
    if (s->kind) {
        if (CanEnter(&s->position, s->cell, next, angle))
            return 0;
        cellp = &s->cell;
        position = &s->position;
        quadrant = angle + 0x20;
        if (gEngineFlags & 0x800) {
            s32 target;
            cell = *cellp;
            switch ((quadrant >> 6) & 3) {
                case 0:
                case 2:
                    if (Seek(position, cell, cell - SceneGrid_Width())) {
                        result = 3;
                        break;
                    }
                    if (!Seek(position, cell, cell + SceneGrid_Width()))
                        goto stop;
                    result = 1;
                    break;
                case 1:
                case 3:
                    target = cell + 1;
                    if (Seek(position, cell, target)) {
                        result = 0;
                        break;
                    }
                    target = cell - 1;
                    {
                        u8 found = Seek(position, cell, target);
                        result = 2;
                        if (found)
                            goto check;
                        goto stop;
                    }

                default:
                    goto stop;
            }
        } else {
            u8 found[4];
            struct BouncePosition delta;
            s32 start = *cellp;
            s32 direction = (quadrant >> 6) & 3;
            s32 preferred;
            s32 target;
            struct AreaEffectPlayer *player = gUnk_02000580[0];
            if (!player)
                preferred = 0;
            else {
                Delta(&delta, (struct BouncePosition *)&player->position, position);
                preferred = ((Math_Atan2(delta.x, delta.z) + 0x20) & 0xFF) >> 6;
            }
            Memory_Clear(found, 4);
            switch (direction) {
                case 0:
                case 2:
                    if (Seek(position, start, start - SceneGrid_Width()))
                        found[3] = 1;
                    if (Seek(position, start, start + SceneGrid_Width()))
                        found[1] = 1;
                    break;
                case 1:
                case 3:
                    target = start + 1;
                    if (Seek(position, start, target))
                        found[0] = 1;
                    target = start - 1;
                    if (Seek(position, start, target))
                        found[2] = 1;
                    break;
            }
            if (found[preferred] == 1)
                result = preferred;
            else {
                s32 i;
                for (i = 0; i <= 3; i++) {
                    if (found[i]) {
                        result = i;
                        goto check;
                    }
                }
                result = -1;
            }
        }
    check:
        if (result < 0)
            goto stop;
        {

            s32 vx, vz;
            vx = s->vx;
            if (vx < 0)
                vx = -vx;
            vz = s->vz;
            if (vz < 0)
                vz = -vz;
            speed = vz;
            if (speed < vx)
                speed = vx;
            switch (result) {
                case 0:
                    s->vx = speed;
                    s->vz = 0;
                    break;
                case 1:
                    s->vx = 0;
                    s->vz = speed;
                    break;
                case 2:
                    s->vx = -speed;
                    s->vz = 0;
                    break;
                case 3:
                    s->vx = 0;
                    s->vz = -speed;
                    break;
            }
            return 1;
        }
    } else if (!CanEnter(&s->position, s->cell, next, angle)) {
    stop:
        s->state = 2;
        return 1;
    }
    return 0;
}
