#include "global.h"
struct DisplayAnim {
    u8 pad0[0x8c];
    u16 *sprite;
    u8 pad90[4];
};
struct PaletteTransition {
    u8 pad0[0x18];
    s32 phase;
    u8 pad1c[0xc];
    struct DisplayAnim anim[2];
    u8 pad150[0x9ee - 0x150];
    u16 base;
    u8 pad9f0[0xc];
    u16 index;
    u8 dirX;
    u8 dirY;
};
extern const s16 gUnk_086149C4[];
int DisplayAnimation_SetEntry(void *, int, int, int, int, int, int);
int DisplayAnimation_UpdatePlayback(void *);
static inline s32 shift5(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline s32 shift13(s32 value)
{
    if (value >= 0)
        return value >> 13;
    return -((-value) >> 13);
}
void PaletteTransition_UpdateWobble(struct PaletteTransition *p)
{
    s32 i;
    s32 delta;
    s32 angle;
    for (i = 0; i < 2; i++) {
        struct DisplayAnim *anim = &p->anim[i];
        DisplayAnimation_SetEntry(anim, p->index + i, p->dirX, p->dirY, 1, 0, 5);
        DisplayAnimation_UpdatePlayback(anim);
    }
    p->phase += 0x20;
    angle = shift5(p->phase) + 0x80;
    delta = shift13(gUnk_086149C4[angle & 0xff] * 8);
    p->anim[1].sprite[1] = p->base + delta;
    p->anim[0].sprite[1] = p->anim[1].sprite[1];
}
