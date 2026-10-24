// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
struct CloudPosition {
    u16 x, y, z, pad;
};
struct CloudParticle {
    u8 pad0[0x14];
    struct CloudPosition origin;
    struct CloudPosition *anchor;
    u8 pad20[0x28];
};
struct CloudTask {
    u8 pad0[0x30];
    struct CloudParticle particles[40];
};
extern struct CloudTask *gUnk_02000298;
extern u32 gUnk_03005308;
static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return table[index];
}
static inline s32 ShiftAnchor(s32 coordinate, u32 random)
{
    s32 shifted = coordinate - 512;
    asm("" : : "r"(shifted));
    return shifted + (random & 1023);
}
// confidence=low: scatters particle anchors horizontally and raises them by sixteen units.
#define PlayerParticleCloud_ScatterAnchors sub_081D3620
inline static s32 ShiftAnchor_081D3620(s32 coordinate, u32 random)
{
    u8 shifted = coordinate - 512;
    asm("" : : "r"(shifted));
    return (random & 1023) + shifted;
}

s32 PlayerParticleCloud_ScatterAnchors(void)
{
    struct CloudTask *p = gUnk_02000298;
    struct CloudParticle *particle;
    const u16 *table;
    s32 i;
    if (!p)
        return -1;
    particle = p->particles;
    table = (const u16 *)0x0203B400;
    for (i = 39; i >= 0; i--, particle++) {
        u32 random = RandomValue(table);
        particle->origin.x = ShiftAnchor_081D3620(particle->origin.x, random);
        random = RandomValue(table);
        particle->origin.z = ShiftAnchor_081D3620(particle->origin.z, random);
        particle->origin.y += 4096;
        particle->anchor = &particle->origin;
    }

    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081D3620);
#endif
