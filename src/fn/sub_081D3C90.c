#include "global.h"
struct WavePart {
    u8 pad0[0x8C];
    s16 *position;
    u8 pad90[4];
};
struct WaveChain {
    u8 pad0[0x18];
    s32 phase;
    u8 pad1C[0x28 - 0x1C];
    struct WavePart parts[6];
    u8 pad3A0[0x9EC - 0x3A0];
    s16 x, y, z;
};
extern const s8 gUnk_086126CC[];
extern const s16 gUnk_086149C4[];
static inline s32 Shift5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
static inline s32 Shift13(s32 v)
{
    return v >= 0 ? v >> 13 : -((-v) >> 13);
}
// confidence=low: places six parts at fixed offsets from the anchor and sways each one in x by
// a sine wave whose amplitude shrinks along the chain.
#ifdef NONMATCHING
// fuzzy: 43.8%
// fix.py best (sweep after the inlined-insn fix)
// The ROM reloads p->phase every iteration (not hoisted) and keeps the anchor coordinate addresses in registers.
inline static s32 Shift5_081D3C90(s32 v)
{
    u32 k1;
    return (v >= 0) ? (v >> (k1 = 5)) : (-((-v) >> k1));
}

void sub_081D3C90(struct WaveChain *p)
{
    s32 i;
    p->phase += 0x20;
    for (i = 0; i < 6; i++) {
        s32 x = p->x + gUnk_086126CC[i * 2];
        s32 y = p->y + gUnk_086126CC[(i * 2) + 1];
        s32 z = p->z;
        s16 *position = p->parts[i].position;
        position[0] = x;
        position[1] = y;
        position[2] = z;
        {
            u8 k1 = 8;
            p->parts[i].position[0] +=
                Shift13((k1 - i) * gUnk_086149C4[(Shift5_081D3C90(p->phase) + (i * 32)) & 0xFF]);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081D3C90);
#endif
