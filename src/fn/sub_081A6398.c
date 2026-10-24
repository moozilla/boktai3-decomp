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
static inline void ClearWords(const void *src, void *dst, u32 control)
{
    CpuSet(src, dst, control);
}

void sub_081A6398(u8 *p)
{
    u8 *base = p + 0x48;
    ParticleOffset center;
    ParticleOffset offset;
    u32 fill;
    s32 i;
    u32 *src = &fill;
    ParticleOffset *dst = &center;
    fill = 0;
    ClearWords(src, dst, 0x05000002);
    for (i = 0; i < 4; i++) {
        const u16 *table = gUnk_0203B400;
        u16 index = (gUnk_03005308 + 1) & 0x3FF;
        {
            u16 random = table[index];
            offset.x = ((random & 0xFF) + center.x) - 0x80;
        }
        offset.y = center.y;
        index = (index + 1) & 0x3FF;
        gUnk_03005308 = index;
        {
            u16 random = table[index];
            offset.z = ((random & 0xFF) + center.z) - 0x80;
        }
        table = 0;
        SceneParticle_CreateParticles(base + 0xC, &offset, table, table, 1, 1);
    }
}
