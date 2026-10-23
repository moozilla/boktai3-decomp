#include "global.h"
#include "terrain_cell_probe.h"
extern const s16 gUnk_086149C4[];
void Player_UpdateFacingSide(void *);
void ActorSequence_Begin(void *, u32, u32);
struct FacingPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline u32 Height(struct Pos16 *p)
{
    u8 *props = cellProps(cellIndexAt(p));
    u32 kind = *props >> 4;
    u32 h = (*props & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    h -= *(u8 *)&p->z;
    goto done;
slopeX:
    h -= *(u8 *)&p->x;
done:
    return h;
}
#ifdef NONMATCHING
// fuzzy: 89.5%
// Remaining differences: heading allocation and CSE across the reversing-facing tail.
// confidence=low: compares nearby terrain height and input heading before reversing facing.
#define Player_CheckFacingTerrainClearance sub_08156DAC
s32 Player_CheckFacingTerrainClearance(u8 *p)
{
    struct FacingPosition pos;
    s32 direction, left, right, input;
    if (*(u8 *)(p + 0x4E3))
        return 1;
    direction = *(u8 *)(p + 0x4A5);
    left = (direction + 7) & 7;
    right = (direction + 9) & 7;
    input = *(s16 *)(p + 0x34C);
    if (input == direction || input == left || input == right) {
        s32 angle = ((*(u8 *)(p + 0x4A5) + 1) & 7) << 5;
        pos = *(struct FacingPosition *)(p + 0x30);
        pos.x -= scale(50 * gUnk_086149C4[(angle + 64) & 255]);
        pos.z -= scale(50 * gUnk_086149C4[angle]);
        if (Height((struct Pos16 *)&pos) >= (u32)(*(s16 *)(p + 0x32) + 200))
            return 1;
        {
            s32 facing = (*(u8 *)(p + 0x4A5) + 4) & 7;
            *(u8 *)(p + 0x3A4) = facing;
        }
    } else {
        s32 facing = (direction + 4) & 7;
        *(u8 *)(p + 0x3A4) = facing;
    }
    Player_UpdateFacingSide(p);
    ActorSequence_Begin(p, 0, 0);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08156DAC);
#endif
