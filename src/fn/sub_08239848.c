#include "link_player_palette.h"

static inline u16 BlendLinkColor(const u16 *source, const u16 *base, const u8 *brightnessPtr,
                        s32 sourceWeight, s32 baseWeight, s32 shift)
{
    s32 r, g, b, brightness, red;
    s32 br, bg, bb;
    red = *source & 31;
    brightness = *brightnessPtr;
    r = red + brightness;
    if (r > 31)
        r = 31;
    g = ((*source >> 5) & 31) + brightness;
    if (g > 31)
        g = 31;
    b = ((*source >> 10) & 31) + brightness;
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
void LinkPlayer_BlendPalette(struct LinkPlayerFade *p, const u16 *input, s32 sourceWeight,
                             s32 baseWeight, s32 shift)
{
    const u16 *source;
    u16 *base;
    s32 i;
    const u8 *brightnessPtr;
    const u8 *alternateBrightness;
    if (sourceWeight == 0) {
        if (!p->alternate)
            p->palette = p->basePalette;
        else
            p->target->palette = p->basePalette;
        return;
    }
    source = input;
    if (!p->alternate) {
        u16 **palette;
        i = 0;
        palette = &p->palette;
        base = p->basePalette;
        brightnessPtr = &p->brightness;
        for (; i <= 15; i++) {
            if (i == 5 || i == 6 || i == 13)
                base[16] = *base;
            else
                base[16] = BlendLinkColor(source, base, brightnessPtr, sourceWeight, baseWeight, shift);
            source++;
            base++;
        }
        *palette = p->workPalette;
    } else {
        i = 0;
        alternateBrightness = &p->brightness;
        base = p->basePalette;
        while (1) {
            base[16] = BlendLinkColor(source, base, alternateBrightness, sourceWeight, baseWeight, shift);
            source++;
            base++;
            if (++i > 15)
                break;
        }
        p->target->palette = p->workPalette;
    }
}
