#include "global.h"
#include "engine.h"

struct ChallengeFrames {
    u32 frames[4];
};
extern const struct ChallengeFrames gUnk_0824F9F0;
void CpuSet(const void *, void *, u32);

void sub_081C5B6C(u8 *p)
{
    struct ChallengeFrames frames = gUnk_0824F9F0;
    s32 i;
    u8 *source = p + 0x38;
    u32 *frame = frames.frames;
    u8 *animation = p + 0x78;
    for (i = 0; i < 4; i++) {
        Anim_LoadKeyframe(animation, p + 0x38, (u16)*frame++, 0);
        animation += 0x60;
    }
    {
        u16 *position = (u16 *)(p + 0x158);
        u8 zero = 0;
        u32 v;
        *position = 0x92;
        *(u16 *)(p + 0x15a) = v = 0xFFE8;
        Anim_LoadKeyframe(p + 0x78, source, (u16)(*(u32 *)(p + 0x2fc) + 0x50), 0);
        *(u16 *)(p + 0x112) = v = 0xD008;
        *(u8 **)(p + 0x120) = p + 0x2c0;
        CpuSet(gPaletteBankData + 0x64E0, p + 0x2c0, 0x04000008);
        *(u8 *)(p + 0x2e0) = zero;
    }
}
