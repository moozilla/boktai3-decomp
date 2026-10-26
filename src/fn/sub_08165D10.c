// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
struct PaletteBlendHud {
    u8 f0[0xA18];
    u16 buf[16];
    u16 from;
    u16 to;
    u8 frame;
    u8 duration;
    u8 active;
};
s32 Div(s32, s32);
s32 SceneHud_TickPaletteBlend(struct PaletteBlendHud *p0)
{
    struct PaletteBlendHud *p = p0;
    u16 *fromPalette;
    u16 *toPalette;
    s32 colorIndex;
    s32 remainingFrames;
    u8 *framep;
    u8 *durp;
    u32 channelShift = 5;
    if (!p->active)
        return 1;
    fromPalette = (u16 *)(((u8 *)gPaletteBankData) + (p->from << channelShift));
    toPalette = (u16 *)(((u8 *)gPaletteBankData) + (p->to << channelShift));
    framep = &p->frame;
    ++*framep;
    durp = &p->duration;
    if (*framep >= *durp) {
        u16 *dst = p->buf;
        colorIndex = 15;
        do {
            *dst = *toPalette;
            toPalette++;
            dst++;
            colorIndex--;
        } while (colorIndex >= 0);
        p->active = 0;
        return 1;
    }
    remainingFrames = (*durp) - (*framep);
    {
        for (colorIndex = 0; colorIndex <= 15; colorIndex++) {
            u8 *framep = &p->frame;
            u8 *durp = &p->duration;
            {
                if ((colorIndex != 5) && (colorIndex != 13)) {
                    u32 fromColor = *fromPalette;
                    u32 fromRed;
                    u32 fromGreen;
                    u32 fromBlue;
                    u32 toColor;
                    u32 toRed;
                    u32 toGreen;
                    u32 toBlue;
                    u32 mask;
                    s32 red;
                    s32 green;
                    s32 blue;
                    fromRed = 31;
                    fromRed &= fromColor;
                    fromGreen = (fromColor << 16) >> 21;
                    mask = 31;
                    fromGreen &= mask;
                    fromBlue = ((fromColor << 16) >> 26) & mask;
                    toColor = *toPalette;
                    toRed = 31;
                    asm("" : "+r"(toRed));
                    toRed &= toColor;
                    toGreen = ((toColor << 16) >> 21) & mask;
                    toBlue = ((toColor << 16) >> 26) & mask;
                    red = Div((fromRed * remainingFrames) + (toRed * (*framep)), *durp);
                    green = Div((fromGreen * remainingFrames) + (toGreen * (*framep)), *durp);
                    blue = Div((fromBlue * remainingFrames) + (toBlue * (*framep)), *durp);
                    p->buf[colorIndex] = ((blue << 10) | (green << channelShift)) | red;
                }
            }
            fromPalette++;
            toPalette++;
        }
    }
    return 0;
}
