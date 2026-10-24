#include "global.h"
typedef struct ParticleOffset {
    u32 x : 16, y : 16, z : 16, pad : 16;
} ParticleOffset;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 SceneParticle_CreateParticles(const void *anchor, const ParticleOffset *offset,
                                  const void *range, const void *jitter, s32 count, u32 priority);
// confidence=low: emits four particles from the anchor at p + 0x54, each offset by a random
// -128..127 in x and z.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM keeps two registers holding &center (x read through one, y and z through the other), so the table and 0xFF are not hoisted.
void sub_081A6398(u8 *p)
{
    u8 *base = p + 0x48;
    ParticleOffset center;
    ParticleOffset offset;
    u32 fill;
    s32 i;
    fill = 0;
    CpuSet(&fill, &center, 0x05000002);
    for (i = 0; i < 4; i++) {
        u8 index = (gUnk_03005308 + 1) & 0x3FF;
        {
            typeof(gUnk_0203B400[index]) v1 = gUnk_0203B400[index];
            offset.x = ((v1 & 0xFF) + center.x) - 0x80;
        }
        offset.y = center.y;
        index = (index + 1) & 0x3FF;
        gUnk_03005308 = index;
        offset.z = ((gUnk_0203B400[index] & 0xFF) + center.z) - 0x80;
        SceneParticle_CreateParticles(base + 0xC, &offset, 0, 0, 1, 1);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A6398);
#endif
