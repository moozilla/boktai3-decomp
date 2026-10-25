#include "global.h"
#ifdef NONMATCHING
// fuzzy: 67.9%
// fix.py best (sweep after the inlined-insn fix)
struct LinkPalTarget {
    u8 pad0[0xC];
    u16 *palette;
};
struct LinkPlayerPalette {
    u8 pad0[0xD4];
    u16 *palette;
    u8 padD8[0xA0];
    struct LinkPalTarget *target;
    u8 pad17C[0x1E4];
    u16 base[16], work[16];
    u8 pad3A0[0x79];
    u8 alternate;
    u8 pad41A[0x6AC];
    u8 brightness;
};
static inline u16 Blend(u16 source, const u16 *base, const u8 *brightnessPtr, s32 sourceWeight,
                        s32 baseWeight, s32 shift)
{
    s32 r, g, b, brightness;
    u32 packed;
    s32 br, bg, bb;
    r = source & 31;
    brightness = *brightnessPtr;
    r += brightness;
    if (r > 31)
        r = 31;
    packed = (u32)source << 16;
    g = ((packed >> 21) & 31) + brightness;
    if (g > 31)
        g = 31;
    b = ((packed >> 26) & 31) + brightness;
    if (b > 31)
        b = 31;
    br = *base & 31;
    bg = (*base >> 5) & 31;
    bb = (*base >> 10) & 31;
    r = (sourceWeight * r + baseWeight * br) >> shift;
    g = (sourceWeight * g + baseWeight * bg) >> shift;
    b = (sourceWeight * b + baseWeight * bb) >> shift;
    return (b << 10) | (g << 5) | r;
}
// confidence=low: blends palette channels, preserving selected entries for the primary form.
#define LinkPlayer_BlendPalette sub_08239848
inline static u16 Blend_08239848(u16 source, const u16 *base, const u8 *brightnessPtr,
                                 s32 sourceWeight, s32 baseWeight, s32 shift)
{
    s32 r;
    s32 g;
    s32 b;
    s32 brightness;
    u32 packed;
    s32 br;
    s32 bg;
    s32 bb;
    r = source & 31;
    brightness = *brightnessPtr;
    r += brightness;
    if (r > 31)
        r = 31;
    packed = ((u32)source) << 16;
    g = ((packed >> 21) & 31) + brightness;
    if (g > 31)
        g = 31;
    b = ((packed >> 26) & 31) + brightness;
    if (b > 31)
        b = 31;
    br = (*base) & 31;
    bg = ((*base) >> 5) & 31;
    bb = ((*base) >> 10) & 31;
    r = ((sourceWeight * r) + (baseWeight * br)) >> shift;
    g = ((g * sourceWeight) + (baseWeight * bg)) >> shift;
    b = ((sourceWeight * b) + (baseWeight * bb)) >> shift;
    return ((b << 10) | (g << 5)) | r;
}

void LinkPlayer_BlendPalette(struct LinkPlayerPalette *p, const u16 *source, s32 sourceWeight,
                             s32 baseWeight, s32 shift)
{
    s32 i;
    if (sourceWeight == 0) {
        if (!p->alternate)
            p->palette = p->base;
        else
            p->target->palette = p->base;
        return;
    }
    if (!p->alternate) {
        u16 *base = p->base;
        for (i = 0; i <= 15; i++) {
            if ((((u32)(i - 5)) <= 1) || (i == 13))
                base[16] = *base;
            else
                base[16] =
                    Blend_08239848(*source, base, &p->brightness, sourceWeight, baseWeight, shift);
            base++;
            source++;
        }

        p->palette = p->work;
    } else {
        u16 *base = p->base;
        for (i = 0; i <= 15; i++) {
            base[16] =
                Blend_08239848(*source, base, &p->brightness, sourceWeight, baseWeight, shift);
            base++;
            source++;
        }

        p->target->palette = p->work;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08239848);
#endif
