#include "global.h"
#include "render.h"
struct LinkPalTarget {
    u8 pad0[0xc];
    u16 *palette;
};
struct LinkPlayerFade {
    u8 pad0[0xd4];
    u16 *palette;
    u8 padD8[0x178 - 0xd8];
    struct LinkPalTarget *target;
    u8 pad17C[0x360 - 0x17c];
    u16 basePalette[16];
    u16 workPalette[16];
    u8 pad3A0[0x419 - 0x3a0];
    u8 flag;
    u8 pad41A[0xac4 - 0x41a];
    s16 modeArg;
    u8 mode;
    u8 pad_ac7;
    u8 animIdx;
    u8 animIdxNew;
    u8 padACA[0xad6 - 0xaca];
    u16 srcSlot;
    u16 fadeIn;
    u16 dstSlot;
    u16 fadeOut;
};
void LinkPlayer_LoadAnimationParameters(void *);
s32 ActorEffect_SelectModeFlag(void *);
void PaletteFade_ApplySelectiveBrightness(void *);
s32 Player_GetActionIndex(void *);
void sub_08239848(void *, void *, s32, s32, s32);
void sub_08239A20(struct LinkPlayerFade *p, s32 arg)
{
    s32 mode;
    s32 i;
    if (p->animIdxNew != p->animIdx) {
        p->animIdx = p->animIdxNew;
        LinkPlayer_LoadAnimationParameters(p);
    }
    mode = ActorEffect_SelectModeFlag(p);
    if (p->modeArg != arg || p->mode != mode) {
        p->modeArg = arg;
        p->mode = mode;
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
            sub_08239848(p, src, amount, 0x20 - amount, 5);
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
            sub_08239848(p, src, amount, 0x20 - amount, 5);
        } else {
            p->fadeOut = 0;
            if (p->flag == 0)
                p->palette = p->basePalette;
            else
                p->target->palette = p->basePalette;
        }
    } else {
        p->fadeOut = 0;
        if (p->fadeIn > 0x1f) {
            u16 *src = (u16 *)(gPaletteBankData + p->srcSlot * 32);
            if (p->flag == 0) {
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
            sub_08239848(p, gPaletteBankData + p->srcSlot * 32, p->fadeIn, 0x20 - p->fadeIn, 5);
        }
        p->fadeIn--;
    }
}
