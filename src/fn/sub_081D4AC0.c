#include "global.h"

struct BestiaryConfig {
    u8 pad[14];
    u8 animation, flags;
    u32 pad10;
    void *callback;
    s8 x, y;
    u8 amplitude, speed;
    u8 pad1c[4];
    u8 overlay;
    u8 pad21[3];
};
extern const struct BestiaryConfig gUnk_086126E0[];
extern const s16 gUnk_086149C4[];
void DisplayAnimation_SetFlags(void *, u32);
void DisplayAnimation_ClearFlags(void *, u32);
void PaletteTransition_Restart(void *);
void Callback_InvokeActor(void *, void *);
void Dir_Decompose(s32, u8 *, u8 *, u32);
void DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);
void DisplayAnimation_UpdatePlayback(void *);
void DisplayEntry_SetAnim(void *, s32);
static inline s32 Scale(s32 n, s32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
s32 BestiaryDisplay_UpdateEntry(u8 *p)
{
    s32 i;
    u8 *animations;
    s32 off;
    const struct BestiaryConfig *config;
    if (p[0x25]) {
        i = 0;
        animations = p + 0x28;
        for (; i <= 5; i++) {
            off = i * 0x94;
            if (*(u32 *)(animations + off))
                DisplayAnimation_SetFlags(p + 0x28 + off, 1);
        }
        *(u32 *)(p + 0x9a4) |= 1;
        return 0;
    }
    i = 0;
    animations = p + 0x28;
    for (; i <= 5; i++) {
        s32 o2 = i * 0x94;
        if (*(u32 *)(animations + o2))
            DisplayAnimation_ClearFlags(p + 0x28 + o2, 1);
    }
    *(u32 *)(p + 0x9a4) &= ~1;
    {
        u8 *blocked = p + 0xa02;
        u32 zero = 0;
        *blocked = zero;
        config = &gUnk_086126E0[*(u32 *)(p + 0x1c)];
        *(const struct BestiaryConfig **)(p + 0xa08) = config;
        if (p[0xa03]) {
            *(u32 *)(p + 0x18) = zero;
            *(u32 *)(p + 0x20) = zero;
            p[0x24] = zero;
            PaletteTransition_Restart(p);
        }
    }
    if (config->callback)
        Callback_InvokeActor(p, config->callback);
    if (!(config->flags & 4) && !p[0xa02]) {
        Dir_Decompose(config->animation, p + 0x9fe, p + 0x9ff, *(u16 *)(p + 0xa00));
        DisplayAnimation_SetEntry(animations, *(u16 *)(p + 0x9fc), p[0x9fe], p[0x9ff], 1, 0, 5);
        DisplayAnimation_UpdatePlayback(animations);
    }
    if (config->flags & 2) {
        s32 value;
        s32 angle;
        *(s32 *)(p + 0x18) += config->speed;
        angle = Scale(*(s32 *)(p + 0x18), 5) + 128;
        {
            s32 amplitude = config->amplitude;
            value = Scale(amplitude * gUnk_086149C4[angle & 255], 13);
        }
        *(u16 *)(*(u8 **)(p + 0xb4) + 2) = *(u16 *)(p + 0x9ee) + value;
    }
    if (!config->overlay)
        *(u32 *)(p + 0x9a4) |= 1;
    else {
        *(u32 *)(p + 0x9a4) &= ~1;
        DisplayEntry_SetAnim(p, config->overlay);
    }
    p[0xa03] = 0;
}
