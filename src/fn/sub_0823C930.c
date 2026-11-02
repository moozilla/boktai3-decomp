#include "global.h"
#include "engine.h"
#include "player.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Player_SetTetherMode(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(void *, s32, s32);
void Player_PlayLocalSong(s32, u32);
s32 Player_IsFlaggedWithGlobalModifier08(u32);
void Player_EmitPositionEffect(void *, void *, u32, u32, void *, u32, u32);

extern const s16 gUnk_086149C4[];

struct FloorPoint {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16, pad : 16;
};
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline s32 cellIndex(struct FloorPoint *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 groundHeight(struct FloorPoint *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + (index * 4 + 12);
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

static inline u8 *cellData(s32 index)
{
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        return cell + 4;
    return gTerrainMap->cells + (index * 4 + 12);
}
static inline u32 ReadCellFlags(u8 *cell)
{
    u16 *field = (u16 *)(cell + 2);
    u32 value = *field;
    return value;
}
static inline void SetFlags(u32 *field, u32 mask)
{
    u32 bits = mask;
    *field = bits | *field;
}
// confidence=low: terrain probes select sideways motion along the coordinate axis.
void Player_UpdateTerrainSidestep(struct Player *player)
{
    struct Player *p = player;
    struct PackedVec pt;
    struct PackedVec effect;
    s32 anim, direction, a, b;
    s32 height, index, speed;
    const s16 *trig;
    player->tetherMode = 4;
    if (p->tetherPhase == 1 && player->inputKeys[1] & 1)
        Player_SetTetherMode(p, 4, 2);
    switch (p->tetherPhase) {
        case 0:
            SetFlags((u32 *)((u8 *)p + 0x20), 4);
            anim = ActorVariant_GetTableValue(p->actionMode, 2);
            if (p->animationIndex != anim) {
                Player_PlayLocalSong(p->slot, 0xD0);
                {
                    u32 facing = (p->coordinateAxis + 4) & 7;
                    p->facing = facing;
                }
                if (p->coordinateAxis > 4) {
                    {
                        s32 side = (8 - p->coordinateAxis) >> 1;
                        p->heading = side;
                    }
                    p->side = 1;
                } else {
                    {
                        u32 side = p->coordinateAxis >> 1;
                        p->heading = side;
                    }
                    p->side = 0;
                }
            }
            if (Player_SetAnimationIndex(p, anim, 0x40))
                Player_SetTetherMode(p, 4, 1);
            break;
        case 1:
            SetFlags((u32 *)((u8 *)p + 0x20), 8);
            p->actionFourEnabled = 1;
            direction = (s16)p->directionHistory[0];
            a = (p->coordinateAxis + 7) & 7;
            b = (p->coordinateAxis + 1) & 7;
            anim = ActorVariant_GetTableValue(p->actionMode, 3);
            if (direction == a || direction == b) {
                {
                    s32 delta = (direction - p->coordinateAxis) * 2 + 8;
                    direction = (p->coordinateAxis + delta) & 7;
                }
                direction = (direction + 1) & 7;
                pt = *(struct PackedVec *)((u8 *)p + 0x30);
                {
                    s32 product = gUnk_086149C4[(direction * 32 + 64) & 255] * 50;
                    trig = gUnk_086149C4;
                    pt.x -= Scale(product);
                }
                pt.z -= Scale(trig[(direction * 32) & 255] * 50);
                index = cellIndex((void *)&pt);
                height = groundHeight((void *)&pt);
                if (height > p->position.y - 128 && height < p->position.y + 128 &&
                    !Player_IsFlaggedWithGlobalModifier08(*(u16 *)(cellData(index) + 2))) {
                    {
                        s32 product =
                            gUnk_086149C4[((((p->coordinateAxis + 1) & 7) * 32) + 64) & 255] * 50;
                        trig = gUnk_086149C4;
                        ((struct FloorPoint *)&pt)->x -= Scale(product);
                    }
                    ((struct FloorPoint *)&pt)->z -=
                        Scale(trig[((p->coordinateAxis + 1) & 7) * 32] * 50);
                    if ((s32)groundHeight((void *)&pt) >= height + 200) {
                        u32 flags = ReadCellFlags(cellData(cellIndex((void *)&pt)));
                        flags &= 4;
                        if (!flags) {
                            anim = ActorVariant_GetTableValue(p->actionMode, 4);
                            {
                                s32 frame = p->actionCueFlags;
                                speed = 0;
                                if (frame != 0) {
                                    speed = 3;
                                    if (frame == 2)
                                        speed = 6;
                                }
                            }
                            {
                                s32 product = gUnk_086149C4[(direction * 32 + 64) & 255] * speed;
                                trig = gUnk_086149C4;
                                p->velocityX = -Scale(product);
                            }
                            p->velocityZ = -Scale(trig[(direction * 32) & 255] * speed);
                        }
                    }
                }
            }
            Player_SetAnimationIndex(p, anim, 0x40);
            if (p->animationIndex == 24 && p->actionCueFlags == 0 && p->actionCueCounter == 1)
                Player_PlayLocalSong(p->slot, 0xDB);
            break;
        case 2:
            SetFlags((u32 *)((u8 *)p + 0x20), 8);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 5), 0x40)) {
                effect = *(struct PackedVec *)((u8 *)p + 0x30);
                if (p->coordinateAxis == 1) {
                    effect.x += 28;
                    effect.y += 44;
                    effect.z -= 76;
                } else if (p->coordinateAxis == 3) {
                    effect.x -= 52;
                    effect.y += 0;
                    effect.z += 0;
                } else if (p->coordinateAxis == 5) {
                    effect.x += 12;
                    effect.y += 44;
                    effect.z -= 32;
                } else if (p->coordinateAxis == 7) {
                    effect.x -= 64;
                    effect.y += 68;
                    effect.z += 44;
                }
                { /* The object-push variant uses this position for its effect terrain test. */
                    struct PackedVec probe;
                    probe = *(struct PackedVec *)((u8 *)p + 0x30);
                    {
                        const s16 *base = gUnk_086149C4;
                        u32 angle;
                        u8 *heading;
                        s32 sample, product;
                        heading = (u8 *)p + 0x4a5;
                        angle = (*heading + 1) & 7;
                        sample = base[((angle << 5) + 64) & 255];
                        product = sample * 50;
                        trig = base;
                        probe.x -= Scale(product);
                    }
                    {
                        u32 angle;
                        u8 *heading;
                        s32 sample, product;
                        heading = (u8 *)p + 0x4a5;
                        angle = (*heading + 1) & 7;
                        sample = trig[angle << 5];
                        product = sample * 50;
                        probe.z -= Scale(product);
                    }
                }
                Player_EmitPositionEffect(p, (u8 *)p + 0x30, 256, 1, &effect, 0, 236);
                Player_SetTetherMode(p, 4, 1);
            }
            break;
    }
}
