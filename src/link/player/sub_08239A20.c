#include "global.h"
#include "engine/video/render.h"
#include "link/player/link_player_palette.h"

s32 Player_GetActionIndex(void *);

void sub_08239A20(struct LinkPlayerFade *p, s32 arg)
{
    s32 mode;
    s32 i;
    if (p->animIdxNew != p->animIdx) {
        p->animIdx = p->animIdxNew;
        LinkPlayer_LoadAnimationParameters(p);
    }
    mode = ActorEffect_SelectModeFlag(p);
    if (p->modeArg != arg || p->brightness != mode) {
        p->modeArg = arg;
        p->brightness = mode;
        PaletteFade_ApplySelectiveBrightness(p);
    }
    if (p->fadeIn == 0) {
        s32 index = Player_GetActionIndex(p);
        if (index >= 0) {
            const u16 *src;
            s32 amount;
            p->dstSlot = index + 0x16e;
            src = (const u16 *)(gPaletteBankData + p->dstSlot * 32);
            if (p->fadeOut <= 0x1f)
                amount = p->fadeOut;
            else if (p->fadeOut <= 0x2f)
                amount = 0x20;
            else if (p->fadeOut <= 0x4f)
                amount = 0x50 - p->fadeOut;
            else
                amount = 0;
            LinkPlayer_BlendPalette(p, src, amount, 0x20 - amount, 5);
            p->fadeOut++;
            if (p->fadeOut > 0x5f)
                p->fadeOut = 0;
        } else if (p->fadeOut >= 1 && p->fadeOut <= 0x4f) {
            const u16 *src = (const u16 *)(gPaletteBankData + p->dstSlot * 32);
            s32 amount;
            if (p->fadeOut >= 0x21 && p->fadeOut <= 0x2f)
                p->fadeOut = 0x30;
            if (p->fadeOut <= 0x20) {
                amount = p->fadeOut;
                p->fadeOut = amount - 1;
            } else {
                amount = 0x50 - p->fadeOut;
                p->fadeOut++;
                if (p->fadeOut > 0x4f)
                    p->fadeOut = 0;
            }
            LinkPlayer_BlendPalette(p, src, amount, 0x20 - amount, 5);
        } else {
            p->fadeOut = 0;
            if (p->alternate == 0)
                p->palette = p->basePalette;
            else
                p->target->palette = p->basePalette;
        }
    } else {
        p->fadeOut = 0;
        if (p->fadeIn > 0x1f) {
            const u16 *src = (const u16 *)(gPaletteBankData + p->srcSlot * 32);
            if (p->alternate == 0) {
                for (i = 0; i < 16; i++) {
                    if (i == 5 || i == 6 || i == 13)
                        p->workPalette[i] = p->basePalette[i];
                    else
                        p->workPalette[i] = *src;
                    src++;
                }
                p->palette = p->workPalette;
            } else {
                u16 *dst = p->workPalette;

                for (i = 15; i >= 0; i--)
                    *dst++ = *src++;
                p->target->palette = p->workPalette;
            }
        } else {
            LinkPlayer_BlendPalette(p, (const u16 *)(gPaletteBankData + p->srcSlot * 32), p->fadeIn,
                                    0x20 - p->fadeIn, 5);
        }
        p->fadeIn--;
    }
}
