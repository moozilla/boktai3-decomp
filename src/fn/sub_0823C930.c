#include "global.h"
#include "engine.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 86.0%
// Terrain sidestep reconstruction; packed vectors follow Player_TryStepForward.
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
    u32 x : 16, y : 16, z : 16, pad : 16;
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 groundHeight(struct FloorPoint *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
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
    return gUnk_030052F4->cells + index * 4 + 12;
}
// confidence=low: terrain probes select sideways motion along the coordinate axis.
void Player_UpdateTerrainSidestep(u8 *p)
{
    struct PackedVec pt;
    struct PackedVec effect;
    s32 anim, direction, a, b;
    s32 height, index, speed;
    B(0x457) = 4;
    if (B(0x458) == 1 && (*(u16 **)(p + 0x348))[1] & 1)
        Player_SetTetherMode(p, 4, 2);
    switch (B(0x458)) {
        case 0:
            W(0x20) |= 4;
            anim = ActorVariant_GetTableValue(B(0x418), 2);
            if (H(0x3A0) != anim) {
                Player_PlayLocalSong(W(0x18), 0xD0);
                B(0x3A4) = (B(0x4A5) + 4) & 7;
                if (B(0x4A5) > 4) {
                    B(0x3A2) = (8 - B(0x4A5)) >> 1;
                    B(0x3A3) = 1;
                } else {
                    B(0x3A2) = B(0x4A5) >> 1;
                    B(0x3A3) = 0;
                }
            }
            if (Player_SetAnimationIndex(p, anim, 0x40))
                Player_SetTetherMode(p, 4, 1);
            break;
        case 1:
            W(0x20) |= 8;
            B(0x4A4) = 1;
            direction = S(0x34C);
            a = (B(0x4A5) + 7) & 7;
            b = (B(0x4A5) + 1) & 7;
            anim = ActorVariant_GetTableValue(B(0x418), 3);
            if (direction == a || direction == b) {
                direction = ((B(0x4A5) + (direction - B(0x4A5)) * 2 + 8) & 7);
                direction = (direction + 1) & 7;
                pt = *(struct PackedVec *)(p + 0x30);
                pt.x -= Scale(gUnk_086149C4[(direction * 32 + 64) & 255] * 50);
                pt.z -= Scale(gUnk_086149C4[(direction * 32) & 255] * 50);
                index = cellIndex((void *)&pt);
                height = groundHeight((void *)&pt);
                if (height > S(0x32) - 128 && height < S(0x32) + 128 &&
                    !Player_IsFlaggedWithGlobalModifier08(*(u16 *)(cellData(index) + 2))) {
                    ((struct FloorPoint *)&pt)->x -=
                        Scale(gUnk_086149C4[((((B(0x4A5) + 1) & 7) * 32) + 64) & 255] * 50);
                    ((struct FloorPoint *)&pt)->z -=
                        Scale(gUnk_086149C4[((B(0x4A5) + 1) & 7) * 32] * 50);
                    if ((s32)groundHeight((void *)&pt) >= height + 200 &&
                        !(*(u16 *)(cellData(cellIndex((void *)&pt)) + 2) & 4)) {
                        anim = ActorVariant_GetTableValue(B(0x418), 4);
                        speed = H(0xA0) == 0 ? 0 : H(0xA0) == 2 ? 6 : 3;
                        S(0x38) = -Scale(gUnk_086149C4[(direction * 32 + 64) & 255] * speed);
                        S(0x3C) = -Scale(gUnk_086149C4[(direction * 32) & 255] * speed);
                    }
                }
            }
            Player_SetAnimationIndex(p, anim, 0x40);
            if (H(0x3A0) == 24 && H(0xA0) == 0 && H(0x98) == 1)
                Player_PlayLocalSong(W(0x18), 0xDB);
            break;
        case 2:
            W(0x20) |= 8;
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(B(0x418), 5), 0x40)) {
                effect = *(struct PackedVec *)(p + 0x30);
                if (B(0x4A5) == 1) {
                    effect.x += 28;
                    effect.y += 44;
                    effect.z -= 76;
                } else if (B(0x4A5) == 3) {
                    effect.x -= 52;
                } else if (B(0x4A5) == 5) {
                    effect.x += 12;
                    effect.y += 44;
                    effect.z -= 32;
                } else if (B(0x4A5) == 7) {
                    effect.x -= 64;
                    effect.y += 68;
                    effect.z += 44;
                }
                Player_EmitPositionEffect(p, p + 0x30, 256, 1, &effect, 0, 236);
                Player_SetTetherMode(p, 4, 1);
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateTerrainSidestep);
#endif
