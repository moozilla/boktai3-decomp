#include "global.h"
struct WavePosition {
    s16 x, y, z, pad;
};
struct WavePart {
    u8 pad0[0x8C];
    struct WavePosition *position;
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
void sub_081D3C90(struct WaveChain *p)
{
    s32 i;
    struct WavePart *part;
    p->phase += 0x20;
    part = p->parts;
    for (i = 0; i < 6; i++) {
        s32 x = p->x;
        s32 dx = gUnk_086126CC[i * 2];
        s32 y = p->y;
        s32 dy = gUnk_086126CC[i * 2 + 1];
        s32 z = p->z;
        struct WavePosition *position = part->position;
        s32 angle;
        x += dx;
        y += dy;
        position->x = x;
        position->y = y;
        position->z = z;
        angle = Shift5(p->phase) + i * 32;
        part->position->x += Shift13((8 - i) * gUnk_086149C4[angle & 255]);
        part++;
    }
}
