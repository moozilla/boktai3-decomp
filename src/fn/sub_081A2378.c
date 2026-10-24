#include "global.h"

#define gRadialSineTable gUnk_086149C4
struct MotionPosition {
    s16 x, y, z, pad;
};
struct Hresvelgr {
    u8 unknown0[0xe4];
    u32 unknownE4;
    u8 unknownE8[0x22c];
    u16 unknown314, unknown316, unknown318, unknown31A;
    u8 unknown31C[8];
    u16 unknown324, unknown326;
    u8 unknown328[0xc];
    u16 unknown334, unknown336;
    struct MotionPosition radial[4];
    u8 unknown358[0x2c];
    u32 unknown384;
};
extern s16 gUnk_086149C4[];
static inline s32 TruncateFixed(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void Hresvelgr_InitializeRadialOffsets(struct Hresvelgr *actor)
{
    s32 i;
    s32 radius;
    actor->unknown334 = 0;
    actor->unknown316 = 0;
    actor->unknown384 = 0;
    actor->unknown324 = 0;
    actor->unknownE4 = 0;
    actor->unknown314 = 0;
    actor->unknown31A = 0;
    i = 0;
    radius = 128;
    for (; i <= 3; i++) {
        struct MotionPosition *position;
        s32 angle;
        s32 z;
        // actor + i * 8 first, then the field offset: as one expression
        // (&actor->radial[i]) fold moves 0x338 into the index sum. angle as a
        // multiply (not i << 6) has fewer references, so position outranks
        // it in register allocation.
        position = (struct MotionPosition *)(i * 8 + (u32)actor);
        position = (struct MotionPosition *)((u8 *)position + 0x338);
        angle = i * 64;
        position->x = TruncateFixed(gUnk_086149C4[(angle + 64) & 255] << 7);
        position->y = 0;
        z = gUnk_086149C4[angle & 255];
        z *= radius;
        position->z = TruncateFixed(z);
    }
}
