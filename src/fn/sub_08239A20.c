#include "global.h"
#include "render.h"
#include "link_player_palette.h"

void LinkPlayer_LoadAnimationParameters(void *);
s32 ActorEffect_SelectModeFlag(void *);
void PaletteFade_ApplySelectiveBrightness(void *);
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
            u8 *src;
            s32 amount;
            p->dstSlot = index + 0x16e;
            src = gPaletteBankData + p->dstSlot * 32;
            if (p->fadeOut <= 0x1f)
                amount = p->fadeOut;
            else if (p->fadeOut <= 0x2f)
                amount = 0x20;
            else if (p->fadeOut <= 0x4f)
                amount = 0x50 - p->fadeOut;
            else
                amount = 0;
            LinkPlayer_BlendPalette(p, (const u16 *)src, amount, 0x20 - amount, 5);
            p->fadeOut++;
            if (p->fadeOut > 0x5f)
                p->fadeOut = 0;
        } else if ((u16)(p->fadeOut - 1) <= 0x4e) {
            u8 *src = gPaletteBankData + p->dstSlot * 32;
            s32 amount;
            if ((u16)(p->fadeOut - 0x21) <= 0xe)
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
            LinkPlayer_BlendPalette(p, (const u16 *)src, amount, 0x20 - amount, 5);
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
            u16 *src = (u16 *)(gPaletteBankData + p->srcSlot * 32);
            if (p->alternate == 0) {
                u16 **slot;
                u16 *dst;
                u16 *base;
                i = 0;
                slot = &p->palette;
                dst = p->workPalette;
                base = p->basePalette;
                for (; i <= 15; i++) {
                    if ((u32)(i - 5) <= 1 || i == 13)
                        *dst = *base;
                    else
                        *dst = *src;
                    src++;
                    dst++;
                    base++;
                }
                *slot = p->workPalette;
            } else {
                u16 *dst = p->workPalette;
                for (i = 15; i >= 0; i--)
                    *dst++ = *src++;
                p->target->palette = p->workPalette;
            }
        } else {
            LinkPlayer_BlendPalette(p, (const u16 *)(gPaletteBankData + p->srcSlot * 32), p->fadeIn, 0x20 - p->fadeIn, 5);
        }
        p->fadeIn--;
    }
}
