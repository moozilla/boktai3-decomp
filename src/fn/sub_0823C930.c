// MATCHING_CONCESSION: register-pinning
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
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))

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
void Player_UpdateTerrainSidestep(u8 *p)
{
    struct PackedVec pt;
    struct PackedVec effect;
    s32 anim, direction, a, b;
    s32 height, index, speed;
    const s16 *trig;
    B(0x457) = 4;
    if (B(0x458) == 1 && (*(u16 **)(p + 0x348))[1] & 1)
        Player_SetTetherMode(p, 4, 2);
    switch (B(0x458)) {
        case 0:
            SetFlags((u32 *)(p + 0x20), 4);
            anim = ActorVariant_GetTableValue(B(0x418), 2);
            if (H(0x3A0) != anim) {
                Player_PlayLocalSong(W(0x18), 0xD0);
                {
                    u32 facing = (B(0x4A5) + 4) & 7;
                    B(0x3A4) = facing;
                }
                if (B(0x4A5) > 4) {
                    {
                        s32 side = (8 - B(0x4A5)) >> 1;
                        B(0x3A2) = side;
                    }
                    B(0x3A3) = 1;
                } else {
                    {
                        u32 side = B(0x4A5) >> 1;
                        B(0x3A2) = side;
                    }
                    B(0x3A3) = 0;
                }
            }
            if (Player_SetAnimationIndex(p, anim, 0x40))
                Player_SetTetherMode(p, 4, 1);
            break;
        case 1:
            SetFlags((u32 *)(p + 0x20), 8);
            B(0x4A4) = 1;
            direction = S(0x34C);
            a = (B(0x4A5) + 7) & 7;
            b = (B(0x4A5) + 1) & 7;
            anim = ActorVariant_GetTableValue(B(0x418), 3);
            if (direction == a || direction == b) {
                {
                    s32 delta = (direction - B(0x4A5)) * 2 + 8;
                    direction = (B(0x4A5) + delta) & 7;
                }
                direction = (direction + 1) & 7;
                pt = *(struct PackedVec *)(p + 0x30);
                {
                    s32 product = gUnk_086149C4[(direction * 32 + 64) & 255] * 50;
                    trig = gUnk_086149C4;
                    pt.x -= Scale(product);
                }
                pt.z -= Scale(trig[(direction * 32) & 255] * 50);
                index = cellIndex((void *)&pt);
                height = groundHeight((void *)&pt);
                if (height > S(0x32) - 128 && height < S(0x32) + 128 &&
                    !Player_IsFlaggedWithGlobalModifier08(*(u16 *)(cellData(index) + 2))) {
                    {
                        s32 product = gUnk_086149C4[((((B(0x4A5) + 1) & 7) * 32) + 64) & 255] * 50;
                        trig = gUnk_086149C4;
                        ((struct FloorPoint *)&pt)->x -= Scale(product);
                    }
                    ((struct FloorPoint *)&pt)->z -= Scale(trig[((B(0x4A5) + 1) & 7) * 32] * 50);
                    if ((s32)groundHeight((void *)&pt) >= height + 200) {
                        u32 flags = ReadCellFlags(cellData(cellIndex((void *)&pt)));
                        flags &= 4;
                        if (!flags) {
                            anim = ActorVariant_GetTableValue(B(0x418), 4);
                            {
                                s32 frame = H(0xA0);
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
                                S(0x38) = -Scale(product);
                            }
                            S(0x3C) = -Scale(trig[(direction * 32) & 255] * speed);
                        }
                    }
                }
            }
            Player_SetAnimationIndex(p, anim, 0x40);
            if (H(0x3A0) == 24 && H(0xA0) == 0 && H(0x98) == 1)
                Player_PlayLocalSong(W(0x18), 0xDB);
            break;
        case 2:
            SetFlags((u32 *)(p + 0x20), 8);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 5), 0x40)) {
                effect = *(struct PackedVec *)(p + 0x30);
                if (B(0x4A5) == 1) {
                    effect.x += 28;
                    effect.y += 44;
                    effect.z -= 76;
                } else if (B(0x4A5) == 3) {
                    effect.x -= 52;
                    effect.y += 0;
                    effect.z += 0;
                } else if (B(0x4A5) == 5) {
                    effect.x += 12;
                    effect.y += 44;
                    effect.z -= 32;
                } else if (B(0x4A5) == 7) {
                    effect.x -= 64;
                    effect.y += 68;
                    effect.z += 44;
                }
                {
                    const s16 *base = gUnk_086149C4;
                    s32 first;
                    s32 sample;
                    u32 mask;
                    register u32 offset asm("r3");
                    u32 angle;
                    u8 *heading;
                    offset = 0x4A5;
                    heading = p + offset;
                    angle = *heading + 1;
                    mask = 7;
                    angle &= mask;
                    sample = base[((angle << 5) + 64) & 255];
                    first = 50;
                    first *= sample;
                    trig = base;
                    asm("" : "+r"(p) : "r"(first), "r"(trig) : "r2", "r3", "memory");
                }
                {
                    s32 second;
                    s32 sample;
                    register u32 mask asm("r1");
                    u32 offset = 0x4A5;
                    u32 angle;
                    u8 *heading;
                    heading = p + offset;
                    angle = *heading + 1;
                    mask = 7;
                    angle &= mask;
                    sample = trig[angle << 5];
                    second = 50;
                    second *= sample;
                    asm("" : : "r"(second));
                }
                Player_EmitPositionEffect(p, p + 0x30, 256, 1, &effect, 0, 236);
                Player_SetTetherMode(p, 4, 1);
            }
            break;
    }
}
