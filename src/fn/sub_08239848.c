#include "link_player_palette.h"

#ifdef NONMATCHING
// fuzzy: 97.0%
static inline u16 BlendLinkColor(u16 source, const u16 *base, const u8 *brightnessPtr,
                                 s32 sourceWeight, s32 baseWeight, s32 shift)
{
    u32 packed;
    s32 red, green, blue, baseRed, baseGreen, baseBlue;
    s32 brightness;
    s32 sourceRed = source & 31;
    brightness = *brightnessPtr;
    red = sourceRed + brightness;
    if (red > 31)
        red = 31;
    packed = (u32)source << 16;
    green = (s16)((packed >> 21) & 31) + brightness;
    if (green > 31)
        green = 31;
    blue = (s16)((packed >> 26) & 31) + brightness;
    if (blue > 31)
        blue = 31;
    baseRed = *base & 31;
    baseGreen = (*base >> 5) & 31;
    baseBlue = (*base >> 10) & 31;
    red = (red * sourceWeight + baseWeight * baseRed) >> shift;
    green = (green * sourceWeight + baseWeight * baseGreen) >> shift;
    blue = (blue * sourceWeight + baseWeight * baseBlue) >> shift;
    return (blue << 10) | (green << 5) | red;
}

void LinkPlayer_BlendPalette(struct LinkPlayerFade *p, const u16 *input, s32 sourceWeight,
                             s32 baseWeight, s32 shift)
{
    s32 i;
    const u16 *source;
    u8 *primaryBrightness;
    u8 *alternateBrightness;
    if (sourceWeight == 0) {
        if (!p->alternate)
            p->palette = p->basePalette;
        else
            p->target->palette = p->basePalette;
        return;
    }
    source = input;
    if (!p->alternate) {
        u16 *base;
        u16 **slot;
        i = 0;
        slot = &p->palette;
        base = p->basePalette;
        primaryBrightness = &p->brightness;
        for (; i < 16; i++) {
            if ((u32)(i - 5) <= 1 || i == 13)
                base[16] = *base;
            else
                base[16] = BlendLinkColor(*source, base, primaryBrightness, sourceWeight,
                                          baseWeight, shift);
            source++;
            base++;
        }
        *slot = p->workPalette;
    } else {
        u16 *base;
        i = 0;
        alternateBrightness = &p->brightness;
        base = p->basePalette;
        while (1) {
            base[16] =
                BlendLinkColor(*source, base, alternateBrightness, sourceWeight, baseWeight, shift);
            source++;
            base++;
            if (++i > 15)
                break;
        }
        p->target->palette = p->workPalette;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", LinkPlayer_BlendPalette);
#endif
