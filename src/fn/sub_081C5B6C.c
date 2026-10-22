#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 98.9%
// Open: the ROM materialises the zero for the final byte store (movs r4, #0) after
// the p+0x158 address; this draft does it first.
struct ChallengeFrames {
    u32 frames[4];
};
extern const struct ChallengeFrames gUnk_0824F9F0;
void CpuSet(const void *, void *, u32);

void sub_081C5B6C(u8 *p)
{
    struct ChallengeFrames frames = gUnk_0824F9F0;
    s32 i;
    for (i = 0; i < 4; i++)
        Anim_LoadKeyframe(p + 0x78 + i * 0x60, p + 0x38, (u16)frames.frames[i], 0);
    {
        u8 zero = 0;
        u32 v;
        *(u16 *)(p + 0x158) = 0x92;
        *(u16 *)(p + 0x15a) = v = 0xFFE8;
        Anim_LoadKeyframe(p + 0x78, p + 0x38, (u16)(*(u32 *)(p + 0x2fc) + 0x50), 0);
        *(u16 *)(p + 0x112) = v = 0xD008;
        *(u8 **)(p + 0x120) = p + 0x2c0;
        CpuSet(gUnk_030042E4 + 0x64E0, p + 0x2c0, 0x04000008);
        *(u8 *)(p + 0x2e0) = zero;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C5B6C);
#endif
